// 
// Core: RateController.h
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

#include <chrono>
#include <thread>

/**
 * \brief Rate Control Helper. Used to controll task execution rate
 */
class RateController {
    using Clock = std::chrono::high_resolution_clock;
public:
    /**
     * \brief Construct an instance with a given execution rate
     * \param rate Exectution Rate
     */
    explicit RateController(const int rate = 0) noexcept : mRate(rate), mDue(Clock::now()), mLast(Clock::now()) {}

    /**
     * \brief Synchronize the internal timer with system clock. For cases that the timer doesn't keep up or forced resets
     */
    void sync() noexcept { mLast = mDue = Clock::now(); }

    /**
     * \brief Get elapsed time from the start of the tick, in milliseconds
     * \return Elapsed time from the start of the tick, in milliseconds
     */
    auto getDeltaTimeMs() const noexcept {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - mLast).count();
    }

    /**
     * \brief Check if the deadline of the current tick has pased
     * \return true if the deadline is passed, false otherwise
     */
    bool isDue() const noexcept { return mRate ? Clock::now() >= mDue : true; }

    /**
     * \brief Increase the internal timer by one tick. Sets the current due time as the starting time of the next tick
     */
    void increaseTimer() noexcept {
        if (mRate) {
            mLast = mDue;
            mDue += std::chrono::milliseconds(1000 / mRate);
        }
    }

    /**
     * \brief End the current tick and wait until the next tick starts
     */
    void yield() noexcept {
        if (!isDue())
            std::this_thread::sleep_until(mDue);
        else
            sync();
        increaseTimer();
    }

private:
    int mRate;
    Clock::time_point mDue, mLast;
};
