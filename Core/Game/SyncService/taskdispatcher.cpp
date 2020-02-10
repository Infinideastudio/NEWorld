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
#include <Cfx/Threading/Micro/ThreadPool.h>
#include <Cfx/Utilities/TempAlloc.h>

namespace {
    enum class DispatchMode : int {
        Read, ReadWrite, None, Render
    };

    ChunkService* gService{nullptr};
    std::atomic_bool gEnter{false};
    thread_local DispatchMode gThreadMode{DispatchMode::None};

    std::vector<std::unique_ptr<ReadOnlyTask>> gReadOnlyTasks, gNextReadOnlyTasks, gRegularReadOnlyTasks;
    std::vector<std::unique_ptr<ReadWriteTask>> gReadWriteTasks, gNextReadWriteTasks, gRegularReadWriteTasks;
    std::vector<std::unique_ptr<RenderTask>> gRenderTasks, gNextRenderTasks;
    std::vector<int64_t> gTimeUsed;
    int64_t gTimeUsedRWTasks;

    SpinLock gReadLock{}, gWriteLock{}, gRenderLock{};

    void ExecuteWriteTasks() noexcept {
        gThreadMode = DispatchMode::ReadWrite;
        for (const auto& task : gReadWriteTasks) { task->task(*gService); }
        gThreadMode = DispatchMode::None;
    }

    void PrepareNextReadOnly() {
        gReadOnlyTasks.clear();
        gReadLock.Enter();
        for (auto& task : gRegularReadOnlyTasks)
            gNextReadOnlyTasks.emplace_back(task->clone());
        std::swap(gReadOnlyTasks, gNextReadOnlyTasks);
        gReadLock.Leave();
    }

    void PrepareNextReadWrite() {
        gReadWriteTasks.clear();
        gWriteLock.Enter();
        for (auto& task : gRegularReadWriteTasks)
            gNextReadWriteTasks.emplace_back(task->clone());
        std::swap(gReadWriteTasks, gNextReadWriteTasks);
        gWriteLock.Leave();
    }

    template <class T>
    [[nodiscard]] auto CountElapsedMs(const T& start) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count();
    }

    void ReadOnlyTaskFinal() noexcept {
        const auto start = std::chrono::steady_clock::now();
        ExecuteWriteTasks();
        PrepareNextReadOnly();
        PrepareNextReadWrite();
        gEnter.store(false);
        gTimeUsedRWTasks = CountElapsedMs(start);
    }

    // This task is scheduled by NRT Thread Pool directly and is responsible
    // for processing NEWorld tasks pool.
    class ExecutePoolTask : public AInstancedExecTask {
    public:
        void Exec(uint32_t instanceId) noexcept override {
            initTick();
            const auto completedTasksNum = drainReads();
            finishTick(instanceId);
            completeTasks(completedTasksNum);
        }

        static void reset() noexcept {
            mCounter = mDone = 0;
            mTotalCount = gReadOnlyTasks.size();
        }

        static void completeTasks(int count) noexcept {
            if (mDone.fetch_add(count) + count == mTotalCount) {
                ReadOnlyTaskFinal();
            }
        }

        static void addTasks(int count) noexcept { mTotalCount.fetch_add(count); }

        static int countCurrentRead() noexcept { return mTotalCount.load(); }
    private:
        void initTick() noexcept {
            mMeter.sync();
            gThreadMode = DispatchMode::Read;
        }

        void finishTick(uint32_t instanceId) noexcept {
            gThreadMode = DispatchMode::None;
            gTimeUsed[instanceId] = mMeter.getDeltaTimeMs();
        }

        [[nodiscard]] static int drainReads() noexcept {
            int localCount = 0;
            for (;;) {
                const auto i = mCounter.fetch_add(1);
                if (i<gReadOnlyTasks.size()) {
                    gReadOnlyTasks[i]->task(*gService);
                    ++localCount;
                }
                else return localCount;
            }
        }
        RateController mMeter{30};

        inline static std::atomic_int mCounter{}, mDone{}, mTotalCount{};
    } gReadPoolTask;

    class MainTimer : public CycleTask {
    public:
        MainTimer() noexcept
                :CycleTask(std::chrono::milliseconds(33)) { }

        void OnTimer() noexcept override {
            auto val = gEnter.exchange(true);
            if (!val) {
                ExecutePoolTask::reset();
                ThreadPool::Spawn(&gReadPoolTask);
            }
        }
    } gMainTimer;

    // This task is scheduled by NRT Thread Pool directly and is responsible
    // for processing a single NEWorld Task.
    class ExecuteSingleTask : public IExecTask {
    public:
        explicit ExecuteSingleTask(std::unique_ptr<ReadOnlyTask> task) noexcept
                :mTask(std::move(task)) { }
        void Exec() noexcept override {
            gThreadMode = DispatchMode::Read;
            mTask->task(*gService);
            gThreadMode = DispatchMode::None;
            mTask.reset();
            ExecutePoolTask::completeTasks(1);
            Temp::Delete(this);
        }

        [[nodiscard]] static IExecTask* create(std::unique_ptr<ReadOnlyTask> task) noexcept {
            ExecutePoolTask::addTasks(1);
            return Temp::New<ExecuteSingleTask>(std::move(task));
        }
    private:
        std::unique_ptr<ReadOnlyTask> mTask;
    };

    struct Service final: NEWorld::Object {
        Service() noexcept {
            gTimeUsed.resize(ThreadPool::CountThreads());
            gService = std::addressof(hChunkService.Get<ChunkService>());
            gEnter.store(false);
        }
        ~Service() noexcept override {
            gMainTimer.Disable();
        }
        NEWorld::ServiceHandle Pool{"org.newinfinideas.nrt.cfx.thread_pool"};
        NEWorld::ServiceHandle Timer{"org.newinfinideas.nrt.cfx.timer"};
        NEWorld::ServiceHandle hChunkService {"org.newinfinideas.neworld.chunk_service" };
    };

    NW_MAKE_SERVICE(Service, "org.newinfinideas.neworld.dispatch", 0.0, _)
}

void TaskDispatch::boot() noexcept {
    gMainTimer.Enable();
}

void TaskDispatch::addNow(std::unique_ptr<ReadOnlyTask> task) noexcept {
    if (gThreadMode==DispatchMode::Read) {
        ThreadPool::Enqueue(ExecuteSingleTask::create(std::move(task)));
    }
    else {
        addNext(std::move(task));
    }
}

void TaskDispatch::addNext(std::unique_ptr<ReadOnlyTask> task) noexcept {
    gReadLock.Enter();
    gNextReadOnlyTasks.emplace_back(std::move(task));
    gReadLock.Leave();
}

void TaskDispatch::addNow(std::unique_ptr<ReadWriteTask> task) noexcept {
    if (gThreadMode==DispatchMode::ReadWrite) {
        task->task(*gService);
    }
    else {
        addNext(std::move(task));
    }
}

void TaskDispatch::addNext(std::unique_ptr<ReadWriteTask> task) noexcept {
    gWriteLock.Enter();
    gNextReadWriteTasks.emplace_back(std::move(task));
    gWriteLock.Leave();
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
    gRenderLock.Enter();
    gNextRenderTasks.emplace_back(std::move(task));
    gRenderLock.Leave();
}

void TaskDispatch::handleRenderTasks() noexcept {
    for (auto& task : gRenderTasks) task->task(*gService);
    gRenderTasks.clear();
    gRenderLock.Enter();
    std::swap(gRenderTasks, gNextRenderTasks);
    gRenderLock.Leave();
}

void TaskDispatch::addRegular(std::unique_ptr<ReadOnlyTask> task) noexcept {
    gReadLock.Enter();
    gRegularReadOnlyTasks.emplace_back(std::move(task));
    gReadLock.Leave();
}

void TaskDispatch::addRegular(std::unique_ptr<ReadWriteTask> task) noexcept {
    gWriteLock.Enter();
    gRegularReadWriteTasks.emplace_back(std::move(task));
    gWriteLock.Leave();
}

int TaskDispatch::countWorkers() noexcept {
    return ThreadPool::CountThreads();
}

int64_t TaskDispatch::getReadTimeUsed(size_t i) noexcept {
    return gTimeUsed.size() > i ? gTimeUsed[i] : 0;
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
    return ExecutePoolTask::countCurrentRead();
}

int TaskDispatch::getReadWriteTaskCount() noexcept {
    return gReadWriteTasks.size();
}
