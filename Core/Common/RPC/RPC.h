// 
// Core: RPC.h
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

#include <utility>
#include <rpc/client.h>
#include <rpc/server.h>
#include "Common/Config.h"

#define FUNCTION_RENAME(oldName, alias)\
    template<typename... Args>\
    auto alias(Args&&... args)->decltype(oldName(std::forward<Args>(args)...))\
    {return oldName(std::forward<Args>(args)...);}

namespace RPC {
    NWCOREAPI void enableClient(const std::string& ip, uint16_t addr);

    NWCOREAPI void enableServer(uint16_t addr);

    NWCOREAPI rpc::client& getClient();

    NWCOREAPI rpc::server& getServer() ;

    // TODO: avoid RPC in singleplayer mode.
    FUNCTION_RENAME(getClient().call, call);
    FUNCTION_RENAME(getClient().async_call, async_call);
    FUNCTION_RENAME(getServer().bind, bind);
};
