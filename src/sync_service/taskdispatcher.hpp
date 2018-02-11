/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <thread>
#include <vector>
#include "sync_service/world/world.h"

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
struct ReadOnlyTask {
    std::function<void(const WorldManager&)> task;
};
/**
 * \brief This type of tasks will be executed in one thread.
 *        Thus, it is safe to do write opeartions inside
 *        without the need to worry thread safety.
 */
struct ReadWriteTask {
    std::function<void(WorldManager&)> task;
};
/**
 * \brief This type of tasks will be executed in main thread.
 *        Thus, it is safe to call OpenGL function inside.
 */
struct RenderTask {
    std::function<void(WorldManager&)> task;
};

template <class TaskType>
struct RegularTask {
    std::function<TaskType()> taskGenerator;
};
using RegularReadOnlyTask = RegularTask<ReadOnlyTask>;
using RegularReadWriteTask = RegularTask<ReadWriteTask>;

class TaskDispatcher {
public:
    /**
     * \brief Initialize the dispatcher and start threads.
     * \param threadNumber The number of threads in the thread pool
     * \param chunkService the chunk service that the dispatcher binds to
     */
    TaskDispatcher(size_t threadNumber, ChunkService& chunkService)
        : mThreadNumber(threadNumber), mChunkService(chunkService) {
        mNumberOfUnfinishedThreads = threadNumber;
        for (size_t i = 0; i < threadNumber; ++i)
            mThreads.emplace_back([this, i]() {worker(i); });
    }
    
    ~TaskDispatcher() {
        mShouldExit = true;
        for (auto& thread : mThreads) thread.join();
    }

    // TODO: NEED FIX! NOT THREAD SAFE!
    void addReadOnlyTask(const ReadOnlyTask& task) noexcept {
        mNextReadOnlyTasks.emplace_back(task);
    }
    void addReadWriteTask(const ReadWriteTask& task) noexcept {
        mNextReadWriteTasks.emplace_back(task);
    }
    void addRenderTask(const RenderTask& task) noexcept {
        mNextRenderTasks.emplace_back(task);
    }
    void addRegularReadOnlyTask(const RegularReadOnlyTask& task) noexcept {
        mRegularReadOnlyTasks.emplace_back(task);
    }
    void addRegularReadWriteTask(const RegularReadWriteTask& task) noexcept {
        mRegularReadWriteTasks.emplace_back(task);
    }

    const std::vector<RenderTask>& getRenderTasks() const noexcept {
        return mRenderTasks;
    }
    void finishedRenderTasks() noexcept {
        mRenderTasks.clear();
        std::swap(mRenderTasks, mNextRenderTasks);
    }
private:
    void worker(size_t threadID);

    std::vector<ReadOnlyTask> mReadOnlyTasks, mNextReadOnlyTasks;
    std::vector<ReadWriteTask> mReadWriteTasks, mNextReadWriteTasks;
    std::vector<RenderTask> mRenderTasks, mNextRenderTasks;
    std::vector<RegularReadOnlyTask> mRegularReadOnlyTasks;
    std::vector<RegularReadWriteTask> mRegularReadWriteTasks;
    std::vector<std::thread> mThreads;
    size_t mThreadNumber;
    std::atomic<size_t> mNumberOfUnfinishedThreads;
    std::atomic<bool> mShouldExit{ false };

    ChunkService& mChunkService;
};