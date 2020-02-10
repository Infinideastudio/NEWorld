// 
// Core: WorldStorage.h
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
#include <string>
#include <sqlite3.h>
#include <fstream>
#include <filesystem>
#include "Common/Logger.h"
#include "world/nwchunk.h"
#include <optional>

class WorldStorage {
public:
    struct WorldStorageIOException : std::exception {};

    WorldStorage(const std::string& worldName) : mWorldName(worldName) {
        if (saveFileExists(worldName)) {
            if (sqlite3_open(getChunkInfoPath(worldName).c_str(), &mWorldInfo)) {
                errorstream << "Failed to open database: " << sqlite3_errmsg(mWorldInfo);
                throw WorldStorageIOException();
            }
        } else {
            debugstream << "World storage database does not exist. Creating...";
            databaseInitialization();
        }
    }

    ~WorldStorage() {
        for (auto& file : mWorldDataFiles) {
            file.flush();
            file.close();
        }
        sqlite3_close(mWorldInfo);
    }

    std::optional<Chunk::ChunkDataStorageType> requestChunk(Vec3i chunkPos);
    void saveChunk(Vec3i chunkPos, const Chunk::ChunkDataStorageType& data);

    static bool saveFileExists(const std::string& worldName) {
        return std::filesystem::exists(getChunkInfoPath(worldName));
    }

private:
    using SQLCallback = int(void*, int, char**, char**);

    struct ChunkInfo {
        size_t file_id, offset, time, size, capacity;
    };

    static constexpr std::string_view BaseWorldPath = "Worlds/";
    static constexpr size_t MaxSizePerFile = 1024 * 1024 * 256; // 256 MB

    static std::string getChunkInfoPath(const std::string& worldName) {
        return std::string(BaseWorldPath) + worldName + ".info";
    }

    static std::string getChunkDataPath(const std::string& worldName, size_t id) {
        return std::string(BaseWorldPath) + worldName + "_" + std::to_string(id) + ".chunks";
    }

    static std::string chunkPosToString(Vec3i pos) {
        return std::to_string(pos.x) + "," + std::to_string(pos.y) + "," + std::to_string(pos.z);
    }

    static int printSQLOutput(void* arg, int cnt, char** vals, char** col);
    static int chunkInfoCallback(void* arg, int cnt, char** vals, char** col);

    void databaseInitialization();

    bool executeSQLCommand(const char* sql, SQLCallback callback, void* argument = nullptr);

    std::optional<ChunkInfo> getChunkInfo(Vec3i chunkPos);

    static size_t getFileSize(std::fstream& file);
    size_t getFirstAvailableFileID(size_t sizeNeeded);
    void insertChunkInfo(Vec3i pos, const ChunkInfo& info);
    void writeChunkToFile(std::fstream& file, const ChunkInfo& info, const Chunk::ChunkDataStorageType& data);

    std::fstream& loadDataFile(size_t id) {
        if (id < mWorldDataFiles.size() && mWorldDataFiles[id].is_open()) return mWorldDataFiles[id];
        mWorldDataFiles.resize(std::max(id + 1, mWorldDataFiles.size()));
        auto fileName = getChunkDataPath(mWorldName, id);
        if (!std::filesystem::exists(fileName))
            std::fstream(fileName, std::ios::out).close();
        return  mWorldDataFiles[id] = std::fstream(fileName,
            std::fstream::in | std::fstream::out | std::fstream::binary);
    }

    std::string mWorldName;
    std::vector<std::fstream> mWorldDataFiles;
    sqlite3* mWorldInfo;
};

