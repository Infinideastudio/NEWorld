// 
// Core: chunkservice.cpp
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

#include "chunkservice.hpp"
#include "Common/Json/JsonHelper.h"

// TODO: set this according to the mode.
NWCOREAPI ChunkService chunkService(true);

ChunkService::ChunkService(bool isAuthority) noexcept
    : mWorlds(context.blocks),
      mTaskDispatcher(getJsonValue<size_t>(getSettings()["nwcore"]["update_thread_number"], 2), *this),
      mAuthority(isAuthority) {}
