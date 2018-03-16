// 
// Core: server.h
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

#include "Common/RateController.h"
#include "Game/SyncService/world/world.h"
#include "Common/Modules.h"

constexpr int UpdateInterval = 1000 / 60, GlobalUpdateInterval = 1000 / 60; // unit: ms

class NWAPIEXPORT Server {
public:
    Server(uint16_t port);
    void run(size_t threadNumber);
    void stop();
    ~Server();
protected:
    RateController mRateCounterScheduler{1};
};
