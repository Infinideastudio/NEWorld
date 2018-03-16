// 
// Core: taskdispatcher.hpp
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

#pragma once
#include <thread>
#include <vector>
#include "Game/SyncService/world/world.h"
#include "Common/Logger.h"

class ChunkService;

// TODO: we can add a `finished` flag in DEBUG mode
//       to verify that all tasks are indeed processed.
/**
 * \brief This type of tasks will be executed concurrently.
 *        Note that "ReadOnly" here is with respect to chunks
 *        data specifically. However please be aware of
 *        thread safety when you write something other than
 *        chunks.
 */
struct NWCOREAPI ReadOnlyTask {
    virtual ~ReadOnlyTask() = default;
    virtual void task(const ChunkService&) = 0;
    virtual std::unique_ptr<ReadOnlyTask> clone() { throw std::runtime_error("Function not implemented"); }
};

/**
 * \brief This type of tasks will be executed in one thread.
 *        Thus, it is safe to do write opeartions inside
 *        without the need to worry thread safety.
 */
struct NWCOREAPI ReadWriteTask {
    virtual ~ReadWriteTask() = default;
    virtual void task(ChunkService&) = 0;
    virtual std::unique_ptr<ReadWriteTask> clone() { throw std::runtime_error("Function not implemented"); }
};

/**
 * \brief This type of tasks will be executed in main thread.
 *        Thus, it is safe to call OpenGL function inside.
 */
struct NWCOREAPI RenderTask {
    virtual ~RenderTask() = default;
    virtual void task(const ChunkService&) = 0;
    virtual std::unique_ptr<RenderTask> clone() { throw std::runtime_error("Function not implemented"); }
};

class NWCOREAPI TaskDispatcher {
public:
    /**
     * \brief Initialize the dispatcher and start threads.
     * \param threadNumber The number of threads in the thread pool
     * \param chunkService the chunk service that the dispatcher binds to
     */
    TaskDispatcher(size_t threadNumber, ChunkService& chunkService)
        : mThreadNumber(threadNumber), mChunkService(chunkService),
          mTimeUsed(threadNumber, 0) { }

    ~TaskDispatcher() { if (!mShouldExit) stop(); }

    void start() {
        mShouldExit = false;
        mNumberOfUnfinishedThreads = mThreadNumber;
        for (size_t i = 0; i < mThreadNumber; ++i)
            mThreads.emplace_back([this, i]() { worker(i); });
        infostream << "Update threads started.";
    }

    void addReadOnlyTask(std::unique_ptr<ReadOnlyTask> task) noexcept {
        std::lock_guard<std::mutex> lock(mMutex);
        mNextReadOnlyTasks.emplace_back(std::move(task));
    }

    void addReadWriteTask(std::unique_ptr<ReadWriteTask> task) noexcept {
        std::lock_guard<std::mutex> lock(mMutex);
        mNextReadWriteTasks.emplace_back(std::move(task));
    }

    void addRenderTask(std::unique_ptr<RenderTask> task) noexcept {
        std::lock_guard<std::mutex> lock(mMutex);
        mNextRenderTasks.emplace_back(std::move(task));
    }

    void addRegularReadOnlyTask(std::unique_ptr<ReadOnlyTask> task) noexcept {
        std::lock_guard<std::mutex> lock(mMutex);
        mRegularReadOnlyTasks.emplace_back(std::move(task));
    }

    void addRegularReadWriteTask(std::unique_ptr<ReadWriteTask> task) noexcept {
        std::lock_guard<std::mutex> lock(mMutex);
        mRegularReadWriteTasks.emplace_back(std::move(task));
    }

    size_t getRegularReadOnlyTaskCount() const noexcept { return mRegularReadOnlyTasks.size(); }
    size_t getRegularReadWriteTaskCount() const noexcept { return mRegularReadWriteTasks.size(); }

    /**
     * \brief Process render tasks.
     *        This function should be called from the main thread.
     */
    void processRenderTasks() {
        std::lock_guard<std::mutex> lock(mMutex);

        for (auto& task : mRenderTasks) task->task(mChunkService);
        mRenderTasks.clear();
        std::swap(mRenderTasks, mNextRenderTasks);
    }

    const std::vector<int64_t>& getTimeUsed() const noexcept { return mTimeUsed; }

    void stop() {
        mShouldExit = true;
        for (auto& thread : mThreads) thread.join();
        infostream << "Update threads exited.";
    }

private:
    void worker(size_t threadID);

    // TODO: replace it with lock-free structure.
    std::mutex mMutex;

    std::vector<std::unique_ptr<ReadOnlyTask>> mReadOnlyTasks, mNextReadOnlyTasks, mRegularReadOnlyTasks;
    std::vector<std::unique_ptr<ReadWriteTask>> mReadWriteTasks, mNextReadWriteTasks, mRegularReadWriteTasks;
    std::vector<std::unique_ptr<RenderTask>> mRenderTasks, mNextRenderTasks;
    std::vector<std::thread> mThreads;
    size_t mThreadNumber;
    std::atomic<size_t> mRoundNumber{0};
    std::atomic<size_t> mNumberOfUnfinishedThreads;
    std::atomic<bool> mShouldExit{false};

    ChunkService& mChunkService;

    std::vector<int64_t> mTimeUsed; // For statistical purpose only
};
