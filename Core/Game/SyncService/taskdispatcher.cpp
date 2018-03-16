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

#include "taskdispatcher.hpp"
#include "chunkservice.hpp"
#include "Common/RateController.h"

void TaskDispatcher::worker(size_t threadID) {
    debugstream << "Worker thread " << threadID << " initialized.";
    RateController meter{30};
    while (!mShouldExit) {
        // A tick starts
        const size_t currentRoundNumber = mRoundNumber;
        // Process read-only work.
        for (auto i = threadID; i < mReadOnlyTasks.size(); i += mThreadNumber) {
            mReadOnlyTasks[i]->task(mChunkService);
        }

        // Finish the tick
        mTimeUsed[threadID] = meter.getDeltaTimeMs();

        // The last finished thread is responsible to do writing jobs
        if (mNumberOfUnfinishedThreads.fetch_sub(1) == 1) {
            // All other threads have finished?
            for (const auto& task : mReadWriteTasks) { task->task(mChunkService); }

            // ...and finish up!
            mReadOnlyTasks.clear();
            mReadWriteTasks.clear();
            for (auto& task : mRegularReadOnlyTasks)
                mNextReadOnlyTasks.emplace_back(task->clone());
            for (auto& task : mRegularReadWriteTasks)
                mNextReadWriteTasks.emplace_back(task->clone());
            std::swap(mReadOnlyTasks, mNextReadOnlyTasks);
            std::swap(mReadWriteTasks, mNextReadWriteTasks);

            // Limit UPS
            meter.yield();

            // Time to move to next tick!
            // Notify other threads that we are good to go
            mNumberOfUnfinishedThreads = mThreadNumber;
            ++mRoundNumber;
        }
        else {
            meter.yield();
            // Wait for other threads...
            while (mRoundNumber == currentRoundNumber)
                std::this_thread::yield();
        }
    }
    debugstream << "Worker thread " << threadID << " exited.";
}
