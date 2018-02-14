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
#include "world/world.h"
#include "game/context/nwcontext.hpp"
#include "taskdispatcher.hpp"

/**
 * \brief This class manages worlds and chunks in NEWorld, and it's responsible
 *        for synchronizing these with the server in multiplayer situation.
 */
class NWCOREAPI ChunkService {
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
    ChunkService(bool isAuthority) noexcept
        : mWorlds(context.plugins, context.blocks),
          mAuthority(isAuthority), mTaskDispatcher(1, *this) {
        // TODO: make thread number adjustable in runtime
    }

    friend class TaskDispatcher;

    TaskDispatcher& getTaskDispatcher() noexcept {
        return mTaskDispatcher;
    }
    
    WorldManager& getWorlds() noexcept { return mWorlds; }
    const WorldManager& getWorlds() const noexcept { return mWorlds; }

    bool isAuthority() const noexcept { return mAuthority; }

private:
    WorldManager mWorlds;
    TaskDispatcher mTaskDispatcher;
    bool mAuthority;
};

// TODO: Hide this and only expose Task Dispatcher.
extern NWCOREAPI ChunkService chunkService;
