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
#include "game/context/nwcontext.hpp"
#include "sync_service/chunkservice.hpp"
#include "rpc/this_handler.h"

std::vector<uint32_t> serverGetChunk(size_t worldID, Vec3i position) {
    // TODO: empty chunk optimization
    auto world = chunkService.getWorlds().getWorld(worldID);
    if (world == nullptr)
        rpc::this_handler().respond_error("The world requested does not exist!");
    Chunk* chunkPtr = nullptr;
    try { chunkPtr = &world->getChunk(position); }
    catch (std::out_of_range&) {
        auto chunk = ChunkManager::data_t(new Chunk(position, *world),
                                          ChunkOnReleaseBehavior::Behavior::Release);
        chunkPtr = world->insertChunkAndUpdate(position, std::move(chunk))->second.get();
    }
    std::vector<uint32_t> chunkData;
    chunkData.resize(32768);
    // It's undefined behavior to use memcpy.
    //    std::memcpy(chunkData.data(), chunkPtr->getBlocks(), sizeof(BlockData) * 32768);
    for (size_t i = 0; i < 32768; ++i)
        chunkData.data()[i] = chunkPtr->getBlocks()[i].getData();
    return chunkData;
}

void registerRPCFunctions() {
    // std::vector<uint32_t> getChunk(size_t worldID, Vec3i position):
    //          Request a chunk from the server.
    //          The chunk will be built or loaded if does not exist.
    context.rpc.getServer().bind("getChunk", &serverGetChunk);

    // std::vector<uint32_t> getAvailableWorldId():
    //          Get all avaliable world ids.
    context.rpc.getServer().bind("getAvailableWorldId",
                                 []() -> std::vector<uint32_t> { return {0}; });

    // std::unordered_map<string, string> getWorldInfo(uint32_t world):
    //          Get world info by id.
    //          "name" : name of the world
    context.rpc.getServer().bind("getWorldInfo",
                                 [](uint32_t worldID) {
                                     std::unordered_map<std::string, std::string> ret;
                                     World* world = chunkService.getWorlds().getWorld(worldID);
                                     if (world == nullptr)
                                         rpc::this_handler().respond_error("The world requested does not exist!");
                                     ret["name"] = world->getWorldName();
                                     return ret;
                                 });
}

Server::Server(uint16_t port) {
    using namespace std::chrono;
    auto startTime = steady_clock::now();
    /*
        infostream << "Initializing nwcore plugins...";
        context.plugins.initializePlugins(nwPluginTypeCore);*/

    infostream << "Initializing server RPC at port " << port << "...";
    context.rpc.enableServer(port);
    registerRPCFunctions();

    infostream << "Initializing map...";
    chunkService.getWorlds().addWorld("test world");

    // Done
    auto doneTime = steady_clock::now();
    infostream << "Initialization done in " << duration_cast<milliseconds>(doneTime - startTime).count() << "ms!";
}

// This function won't block the thread
void Server::run(size_t threadNumber) {
    // Network
    context.rpc.getServer().async_run(threadNumber);
    infostream << "Server RPC started. Thread number: " << threadNumber;
}

void Server::stop() { context.rpc.getServer().stop(); }

Server::~Server() {
    // TODO: Terminate here
}
