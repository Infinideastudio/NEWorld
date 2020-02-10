// 
// Core: chunkservice.hpp
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

#include "Game/SyncService/world/world.h"
#include "taskdispatcher.hpp"
#include "Service.h"

/**
 * \brief This class manages worlds and chunks in NEWorld, and it's responsible
 *        for synchronizing these with the server in multiplayer situation.
 */
class NWCOREAPI ChunkService: public NEWorld::Object {
public:
    /**
     * \brief constructor
     * \param isAuthority if a chunk service is authoritative, its chunk data
     *                    will be used when there are differences between
     *                    chunk data in different chunk services.
     *                    Normally, chunk services in the singleplayer mode
     *                    and in the server-side of the multiplayer mode
     *                    are authoritative.
     */
    ChunkService() noexcept;

    WorldManager& getWorlds() noexcept { return mWorlds; }

    [[nodiscard]] const WorldManager& getWorlds() const noexcept { return mWorlds; }

    [[nodiscard]] bool isAuthority() const noexcept { return mAuthority; }

    void setAuthority(bool isAuthority) noexcept { mAuthority = isAuthority; }
private:
    WorldManager mWorlds;
    bool mAuthority;
};
