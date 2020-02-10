//
// Core: RPC.cpp
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

#include "RPC.h"
#include "Common/Debug.h"
#include "Common/Logger.h"

namespace {
    std::unique_ptr<rpc::client> mClient { nullptr };
    std::unique_ptr<rpc::server> mServer { nullptr };
}
    void RPC::enableClient(const std::string& ip, uint16_t addr) {
        Assert(mClient == nullptr); // Do not initialize the client twice
        mClient = std::make_unique<rpc::client>(ip, addr);
        infostream << "Client connection initialized";
    }

    void RPC::enableServer(uint16_t addr) {
        Assert(mServer == nullptr); // Do not initialize the server twice
        mServer = std::make_unique<rpc::server>(addr);
        debugstream << "Server RPC initialized";
    }

    rpc::client& RPC::getClient() {
        Assert(mClient != nullptr);
        return *mClient;
    }

    rpc::server& RPC::getServer() {
        Assert(mServer != nullptr);
        return *mServer;
    }