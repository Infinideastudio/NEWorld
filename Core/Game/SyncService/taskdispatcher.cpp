// 
// Core: taskdispatcher.cpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
//

#include <vector>
#include "Service.h"
#include "chunkservice.hpp"
#include "taskdispatcher.hpp"
#include "Common/RateController.h"
#include <Cfx/Threading/SpinLock.h>
#include <Cfx/Threading/Micro/Timer.h>
#include <Cfx/Threading/Micro/Promise.h>
#include <Cfx/Threading/Micro/ThreadPool.h>
#include <Cfx/Utilities/TempAlloc.h>

namespace {
    enum class DispatchMode : int {
        Read, ReadWrite, None, Render
    };

    ChunkService* gService{nullptr};
    std::atomic_bool gEnter{false};
    thread_local DispatchMode gThreadMode{DispatchMode::None};

    using ReadTaskHandle = std::unique_ptr<ReadOnlyTask>;
    using ReadWriteTaskHandle = std::unique_ptr<ReadWriteTask>;

    std::vector<ReadTaskHandle> gReadOnlyTasks, gNextReadOnlyTasks, gRegularReadOnlyTasks;
    std::vector<ReadWriteTaskHandle> gReadWriteTasks, gNextReadWriteTasks, gRegularReadWriteTasks;
    std::vector<std::unique_ptr<RenderTask>> gRenderTasks, gNextRenderTasks;
    std::vector<int64_t> gTimeUsed;
    int64_t gTimeUsedRWTasks;

    Lock<SpinLock> gReadLock{}, gWriteLock{}, gRenderLock{};

    void ExecuteWriteTasks() noexcept {
        gThreadMode = DispatchMode::ReadWrite;
        for (const auto& task : gReadWriteTasks) { task->task(*gService); }
        gThreadMode = DispatchMode::None;
        gReadWriteTasks.clear();
    }

    template <class T, class = std::enable_if_t<std::is_convertible_v<T*, InterOp::BasicTask*>>>
    void Cancel(
            std::vector<std::unique_ptr<T>> in,
            std::vector<std::unique_ptr<InterOp::BasicTask>>& remaining
    ) noexcept {
        std::vector<std::unique_ptr<T>> handles = std::move(in);
        for (auto& x : handles) {
            if (x->canCancel()) {
                x->onCancel();
            }
            else {
                remaining.push_back(std::move(x));
            }
        }
    }

    struct Finalizer { Promise<void> Complete; };

    std::atomic<Finalizer*> gFinalizer {nullptr};

    void PrepareNextReadOnly() {
        std::lock_guard lk{gReadLock};
        for (auto& task : gRegularReadOnlyTasks)
            gNextReadOnlyTasks.emplace_back(task->clone());
        std::swap(gReadOnlyTasks, gNextReadOnlyTasks);
    }

    void PrepareNextReadWrite() {
        std::lock_guard lk{gWriteLock};
        for (auto& task : gRegularReadWriteTasks)
            gNextReadWriteTasks.emplace_back(task->clone());
        std::swap(gReadWriteTasks, gNextReadWriteTasks);
    }

    void TaskFinalize(Finalizer* finalizer) noexcept {
        std::vector<std::unique_ptr<InterOp::BasicTask>> buffer {};
        {
            std::lock_guard lk{gReadLock};
            Cancel(std::move(gNextReadOnlyTasks), buffer);
            Cancel(std::move(gRegularReadOnlyTasks), buffer);
        }
        {
            std::lock_guard lk{gWriteLock};
            Cancel(std::move(gNextReadWriteTasks), buffer);
            Cancel(std::move(gRegularReadWriteTasks), buffer);
        }
        while (!buffer.empty()) {
            Cancel(std::move(buffer), buffer);
        }
        finalizer->Complete.SetValueUnsafe();
        Temp::Delete(finalizer);
    }

    void ReadOnlyTaskFinal() noexcept {
        Timer timer{};
        ExecuteWriteTasks();
        if (auto x = gFinalizer.exchange(nullptr); !x) {
            PrepareNextReadOnly();
            PrepareNextReadWrite();
        }
        else {
            TaskFinalize(x);
        }
        gEnter.store(false);
        gTimeUsedRWTasks = timer.getDeltaTimeMs();
    }

    class ReadCompletionControl {
    public:
        static void complete(int count) noexcept {
            if (check(count)) { ReadOnlyTaskFinal(); }
        }

        static void addTasks(int count) noexcept {
            std::lock_guard lk{mLock};
            mTotalCount.store(mTotalCount.load()+count); // explicit non-rmw, only to prevent tearing
        }

        static void frameReset(int count) noexcept {
            std::lock_guard lk{mLock};
            mTotalCount = count;
            mDone = 0;
        }

        static int countTasks() noexcept {
            return mTotalCount.load(std::memory_order::memory_order_relaxed); // only a snapshot
        }
    private:
        static bool check(const int count) noexcept {
            std::lock_guard lk{mLock};
            return (mDone = mDone+count)==mTotalCount; // same as above
        }

        inline static Lock<SpinLock> mLock{};

        inline static std::atomic_int mDone{}, mTotalCount{};
    };

    // This task is scheduled by NRT Thread Pool directly and is responsible
    // for processing NEWorld tasks pool.
    class ReadOnlyExecutor : public AInstancedExecTask {
    public:
        explicit ReadOnlyExecutor(std::vector<ReadTaskHandle>&& handles) noexcept
                :mHandles(std::move(handles)) { }

        void Exec(const uint32_t instance) noexcept override {
            Timer timer{};
            gThreadMode = DispatchMode::Read;
            mCompleteCount.fetch_add(Drain());
            gThreadMode = DispatchMode::None;
            gTimeUsed[instance] = timer.getDeltaTimeMs();
        }

        void OnComplete() noexcept override {
            ReadCompletionControl::complete(mHandles.size());
            mExit.SetValueUnsafe();
            Temp::Delete(this);
        }

        static Future<void> CreateSubmit(std::vector<ReadTaskHandle>&& handles) {
            const auto w = Temp::New<ReadOnlyExecutor>(std::move(handles));
            auto fut = w->mExit.GetFuture();
            ThreadPool::Spawn(w);
            return fut;
        }
    private:
        [[nodiscard]] int Drain() noexcept {
            int localCount = 0;
            for (;;) {
                const auto i = mHead.fetch_add(1);
                if (i<mHandles.size()) {
                    mHandles[i]->task(*gService);
                    ++localCount;
                }
                else return localCount;
            }
        }
        std::vector<ReadTaskHandle> mHandles;
        std::atomic_int mCompleteCount{0}, mHead{0};
        Promise<void> mExit{};
    };

    class FrameControl : public CycleTask {
    public:
        FrameControl() noexcept
                :CycleTask(std::chrono::milliseconds(33)) { }

        void OnTimer() noexcept override {
            auto val = gEnter.exchange(true);
            if (!val) {
                auto vec = queueSwap();
                ReadCompletionControl::frameReset(vec.size());
                ReadOnlyExecutor::CreateSubmit(std::move(vec));
            }
            else {
                debugstream << "Internal Server Tick Lag";
            }
        }
    private:
        static std::vector<ReadTaskHandle> queueSwap() noexcept {
            std::lock_guard lk{gReadLock};
            return std::move(gReadOnlyTasks);
        }
    } gMainTimer;

    // This task is scheduled by NRT Thread Pool directly and is responsible
    // for processing a single NEWorld Task.
    class ExecuteSingleTask : public IExecTask {
    public:
        explicit ExecuteSingleTask(ReadTaskHandle task) noexcept
                :mTask(std::move(task)) { }

        void Exec() noexcept override {
            gThreadMode = DispatchMode::Read;
            mTask->task(*gService);
            gThreadMode = DispatchMode::None;
            mTask.reset();
            ReadCompletionControl::complete(1);
            Temp::Delete(this);
        }

        [[nodiscard]] static IExecTask* create(ReadTaskHandle task) noexcept {
            ReadCompletionControl::addTasks(1);
            return Temp::New<ExecuteSingleTask>(std::move(task));
        }
    private:
        ReadTaskHandle mTask;
    };

    struct Service final : NEWorld::Object {
        Service() noexcept {
            gTimeUsed.resize(ThreadPool::CountThreads());
            gService = std::addressof(hChunkService.Get<ChunkService>());
            gEnter.store(false);
        }
        ~Service() noexcept override {
            auto fin = Temp::New<Finalizer>();
            auto fut = fin->Complete.GetFuture();
            gFinalizer = fin;
            fut.Then([](auto&&) noexcept { gMainTimer.Disable(); }).Wait();
        }
        NEWorld::ServiceHandle Pool{"org.newinfinideas.nrt.cfx.thread_pool"};
        NEWorld::ServiceHandle Timer{"org.newinfinideas.nrt.cfx.timer"};
        NEWorld::ServiceHandle hChunkService{"org.newinfinideas.neworld.chunk_service"};
    };

    NW_MAKE_SERVICE(Service, "org.newinfinideas.neworld.dispatch", 0.0, _)
}

void TaskDispatch::boot() noexcept {
    gMainTimer.Enable();
}

void TaskDispatch::addNow(ReadTaskHandle task) noexcept {
    if (gThreadMode==DispatchMode::Read) {
        ThreadPool::Enqueue(ExecuteSingleTask::create(std::move(task)));
    }
    else {
        addNext(std::move(task));
    }
}

void TaskDispatch::addNext(ReadTaskHandle task) noexcept {
    std::lock_guard lk{gReadLock};
    gNextReadOnlyTasks.emplace_back(std::move(task));
}

void TaskDispatch::addNow(ReadWriteTaskHandle task) noexcept {
    if (gThreadMode==DispatchMode::ReadWrite) {
        task->task(*gService);
    }
    else {
        addNext(std::move(task));
    }
}

void TaskDispatch::addNext(ReadWriteTaskHandle task) noexcept {
    std::lock_guard lk{gWriteLock};
    gNextReadWriteTasks.emplace_back(std::move(task));
}

void TaskDispatch::addNow(std::unique_ptr<RenderTask> task) noexcept {
    if (gThreadMode==DispatchMode::Render) {
        task->task(*gService);
    }
    else {
        addNext(std::move(task));
    }
}

void TaskDispatch::addNext(std::unique_ptr<RenderTask> task) noexcept {
    std::lock_guard lk{gRenderLock};
    gNextRenderTasks.emplace_back(std::move(task));
}

void TaskDispatch::handleRenderTasks() noexcept {
    for (auto& task : gRenderTasks) task->task(*gService);
    gRenderTasks.clear();
    {
        std::lock_guard lk{gRenderLock};
        std::swap(gRenderTasks, gNextRenderTasks);
    }
}

void TaskDispatch::addRegular(ReadTaskHandle task) noexcept {
    std::lock_guard lk{gReadLock};
    gRegularReadOnlyTasks.emplace_back(std::move(task));
}

void TaskDispatch::addRegular(ReadWriteTaskHandle task) noexcept {
    std::lock_guard lk{gWriteLock};
    gRegularReadWriteTasks.emplace_back(std::move(task));
}

int TaskDispatch::countWorkers() noexcept {
    return ThreadPool::CountThreads();
}

int64_t TaskDispatch::getReadTimeUsed(size_t i) noexcept {
    return gTimeUsed.size()>i ? gTimeUsed[i] : 0;
}

int64_t TaskDispatch::getRWTimeUsed() noexcept {
    return gTimeUsedRWTasks;
}

int TaskDispatch::getRegularReadTaskCount() noexcept {
    return gRegularReadOnlyTasks.size();
}

int TaskDispatch::getRegularReadWriteTaskCount() noexcept {
    return gRegularReadWriteTasks.size();
}

int TaskDispatch::getReadTaskCount() noexcept {
    return ReadCompletionControl::countTasks();
}

int TaskDispatch::getReadWriteTaskCount() noexcept {
    return gReadWriteTasks.size();
}

bool InterOp::BasicTask::canCancel() const noexcept { return true; }

void InterOp::BasicTask::onCancel() noexcept { }

void InterOp::TaskNotCloneable() { throw std::runtime_error("Function not implemented"); }
