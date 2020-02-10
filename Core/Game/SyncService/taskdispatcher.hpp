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

#include <memory>
#include <stdexcept>
#include "Common/Config.h"

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

struct TaskDispatch {
    static void NWCOREAPI boot() noexcept;
    // Attempt to add a task to the current task queue if possible.
    // If not, the task will be scheduled to run in the next tick.
    static void NWCOREAPI addNow(std::unique_ptr<ReadOnlyTask> task) noexcept;
    static void NWCOREAPI addNow(std::unique_ptr<ReadWriteTask> task) noexcept;
    static void NWCOREAPI addNow(std::unique_ptr<RenderTask> task) noexcept;

    // Add a task that is scheduled to run in the next tick.
    static void NWCOREAPI addNext(std::unique_ptr<ReadOnlyTask> task) noexcept;
    static void NWCOREAPI addNext(std::unique_ptr<ReadWriteTask> task) noexcept;
    static void NWCOREAPI addNext(std::unique_ptr<RenderTask> task) noexcept;

    // Add regular tasks that run every tick.
    static void NWCOREAPI addRegular(std::unique_ptr<ReadOnlyTask> task) noexcept;
    static void NWCOREAPI addRegular(std::unique_ptr<ReadWriteTask> task) noexcept;

    static void NWCOREAPI handleRenderTasks() noexcept;

    static int NWCOREAPI countWorkers() noexcept;

    static int64_t NWCOREAPI getReadTimeUsed(size_t i) noexcept;

    static int64_t NWCOREAPI getRWTimeUsed() noexcept;

    static int NWCOREAPI getRegularReadTaskCount() noexcept;

    static int NWCOREAPI getRegularReadWriteTaskCount() noexcept;

    static int NWCOREAPI getReadTaskCount() noexcept;

    static int NWCOREAPI getReadWriteTaskCount() noexcept;
};
