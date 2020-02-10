// 
// Core: server.cpp
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

#include "server.h"
#include <Common/RPC/RPC.h>
#include <Common/Logger.h>
#include "Game/SyncService/chunkservice.hpp"
#include "rpc/this_handler.h"

static ChunkService* chunkService;

std::vector<uint32_t> serverGetChunk(size_t worldID, Vec3i position) {
    // TODO: empty chunk optimization
    auto world = chunkService->getWorlds().getWorld(worldID);
    if (world == nullptr)
        rpc::this_handler().respond_error("The world requested does not exist!");
    Chunk* chunkPtr = nullptr;
    try { chunkPtr = &world->getChunk(position); }
    catch (std::out_of_range&) {
        // TODO: FIXME: data race here. Use task dispatcher?
        auto chunk = ChunkManager::data_t(new Chunk(position, *world),
                                          ChunkOnReleaseBehavior::Behavior::Release);
        chunkPtr = world->insertChunkAndUpdate(position, std::move(chunk))->second.get();
    }
    return chunkPtr->getChunkForExport();
}

class PickBlockTask : public ReadWriteTask {
public:
    PickBlockTask(size_t worldID, Vec3i pos)
        : mWorldId(worldID), mPos(pos) { }

    void task(ChunkService& cs) override {
        auto world = cs.getWorlds().getWorld(mWorldId);
        if (!world) return;
        world->setBlock(mPos, BlockData{ 0,0,0 });
    }

private:
    size_t mWorldId;
    Vec3i mPos;
};


Server::Server(uint16_t port) {
    using namespace std::chrono;
    auto startTime = steady_clock::now();
    chunkService = &hChunkService.Get<ChunkService>();

    infostream << "Initializing server RPC at port " << port << "...";
    RPC::enableServer(port);
    registerRPCFunctions();

    infostream << "Initializing map...";
    chunkService->getWorlds().addWorld("test_world");

    // Done
    auto doneTime = steady_clock::now();
    infostream << "Initialization done in " << duration_cast<milliseconds>(doneTime - startTime).count() << "ms!";
}

void Server::registerRPCFunctions() {
    // std::vector<uint32_t> getChunk(size_t worldID, Vec3i position):
    //          Request a chunk from the server.
    //          The chunk will be built or loaded if does not exist.
    // TODO: maybe use task dispatcher?
    RPC::getServer().bind("getChunk", &serverGetChunk);

    // std::vector<uint32_t> getAvailableWorldId():
    //          Get all avaliable world ids.
    RPC::getServer().bind("getAvailableWorldId",
            []() -> std::vector<uint32_t> { return {0}; });

    // std::unordered_map<string, string> getWorldInfo(uint32_t world):
    //          Get world info by id.
    //          "name" : name of the world
    RPC::getServer().bind("getWorldInfo",
            [](uint32_t worldID) {
                std::unordered_map<std::string, std::string> ret;
                World* world = chunkService->getWorlds().getWorld(worldID);
                if (world == nullptr)
                    rpc::this_handler().respond_error("The world requested does not exist!");
                ret["name"] = world->getWorldName();
                return ret;
            });

    // void pickBlock(size_t worldID, Vec3i position):
    //          Remove block at a specific location
    // TODO: add a return value specifying if the operation is successful.
    RPC::getServer().bind("pickBlock",
            [](size_t worldID, Vec3i position){
                TaskDispatch::addNext(std::make_unique<PickBlockTask>(worldID, position));
            });
}
// This function won't block the thread
void Server::run(size_t threadNumber) {
    // Network
    RPC::getServer().async_run(threadNumber);
    infostream << "Server RPC started. Thread number: " << threadNumber;
}

void Server::stop() { RPC::getServer().stop(); }

Server::~Server() {
    // TODO: Terminate here
}
