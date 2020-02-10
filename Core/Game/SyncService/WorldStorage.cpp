#include "WorldStorage.h"
#include <filesystem>
#include <chrono>
namespace fs = std::filesystem;

std::optional<Chunk::ChunkDataStorageType> WorldStorage::requestChunk(Vec3i chunkPos) {
    if (auto pos = getChunkInfo(chunkPos)) {
        auto& file = loadDataFile(pos->file_id);
        auto data = Chunk::ChunkDataStorageType{};
        data.resize(Chunk::BlocksSize);
        file.seekg((*pos).offset);
        file.read(reinterpret_cast<char*>(data.data()), Chunk::BlocksSize * sizeof(BlockData));
        return data;
    }
    return {};
}

void WorldStorage::saveChunk(Vec3i chunkPos, const Chunk::ChunkDataStorageType& data) {
    return;
    auto chunkSize = data.size() * sizeof(BlockData);
    auto allocateSize = chunkSize;

    // Try find the chunk first
    if (auto info = getChunkInfo(chunkPos)) {
        if(chunkSize <= info->capacity) { // Update current chunk data if size <= capacity
            writeChunkToFile(loadDataFile(info->file_id), *info, data);
            // TODO: Update the old record (size specifically if changed).
            return;
        }
        // Remove old chunk data and info if we need to reallocate
        allocateSize = info->capacity * 1.5;
        // TODO: Remove the old chunk. Should not happen now since the size is fixed.
    }

    // Chunk was not previously stored or we need to reallocate.
    ChunkInfo info;
    info.capacity = allocateSize;
    info.size = chunkSize;
    info.file_id = getFirstAvailableFileID(info.capacity);
    info.time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    auto& file = loadDataFile(info.file_id);
    info.offset = getFileSize(file); // Simply put the chunk at the end of the file for now.
    // TODO: Search for available region in the file first.

    // Update data file
    writeChunkToFile(file, info, data);

    // Update record
    insertChunkInfo(chunkPos, info);
}

int WorldStorage::printSQLOutput(void* arg, int cnt, char** vals, char** col) {
    for (int i = 0; i < cnt; i++) {
        debugstream << col[i] << " : " << vals[i];
    }
    return 0;
}

int WorldStorage::chunkInfoCallback(void* arg, int cnt, char** vals, char** col) {
    std::optional<ChunkInfo>* info = static_cast<std::optional<ChunkInfo>*>(arg);
    if (cnt == 0) *info = {};
    else *info = ChunkInfo{};

    for (int i = 0; i < cnt; i++) {
        if (strcmp(col[i], "file_id") == 0) (*info)->file_id = atoi(vals[i]);
        else if (strcmp(col[i], "offset") == 0) (*info)->offset = atoi(vals[i]);
        else if (strcmp(col[i], "capacity") == 0) (*info)->capacity = atoi(vals[i]);
        else if (strcmp(col[i], "size") == 0) (*info)->size = atoi(vals[i]);
        else if (strcmp(col[i], "time") == 0) (*info)->time = atoi(vals[i]);
    }
    return 0;

}

void WorldStorage::databaseInitialization() {
    if (!fs::exists(BaseWorldPath))
        fs::create_directory(BaseWorldPath);
    std::fstream(getChunkInfoPath(mWorldName), std::ios::out).close();
    if (sqlite3_open(getChunkInfoPath(mWorldName).c_str(), &mWorldInfo)) {
        errorstream << "Failed to open database: " << sqlite3_errmsg(mWorldInfo);
        throw WorldStorageIOException();
    }

    executeSQLCommand("CREATE TABLE world_info ("
        "ID INT PRIMARY KEY     NOT NULL,"
        "key           TEXT     NOT NULL,"
        "val         TEXT     NOT NULL);", nullptr);
    executeSQLCommand("CREATE TABLE chunk_info ("
        "position       TEXT  PRIMARY KEY    NOT NULL,"
        "time           INTEGER     NOT NULL,"
        "file_id        INTEGER     NOT NULL,"
        "offset         INTEGER     NOT NULL,"
        "size           INTEGER     NOT NULL,"
        "capacity       INTEGER     NOT NULL);", nullptr);
}

bool WorldStorage::executeSQLCommand(const char* sql, SQLCallback callback, void* argument) {
    char* errMsg;
    auto rc = sqlite3_exec(mWorldInfo, sql, callback, argument, &errMsg);

    if (rc != SQLITE_OK) {
        errorstream << "Error executing SQL command: " << errMsg;
        debugstream << "Command executed was " << sql;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

std::optional<WorldStorage::ChunkInfo> WorldStorage::getChunkInfo(Vec3i chunkPos) {
    std::string sql = "SELECT * from chunk_info where position = \"" + chunkPosToString(chunkPos) + "\"";
    std::optional<ChunkInfo> chunkInfo;
    if (executeSQLCommand(sql.c_str(), chunkInfoCallback, &chunkInfo)){
        return chunkInfo;
    }
    return {};
}

size_t WorldStorage::getFileSize(std::fstream& file) {
    auto original = file.tellg();
    file.seekg(0, std::ios::end);
    auto ret = file.tellg();
    file.seekg(original);
    return ret;
}

size_t WorldStorage::getFirstAvailableFileID(size_t sizeNeeded) {
    for (size_t i = 0; ; i++) {
        auto& file = loadDataFile(i);
        if (getFileSize(file) + sizeNeeded <= MaxSizePerFile) return i;
    }
}

void WorldStorage::insertChunkInfo(Vec3i pos, const ChunkInfo& info) {
    std::string sql = "INSERT INTO chunk_info(position, file_id, offset, time, size, capacity) "
        "VALUES (\"" + chunkPosToString(pos) + "\", " + std::to_string(info.file_id)
        + ", " + std::to_string(info.offset) + "," + std::to_string(info.time) + "," + std::to_string(info.size)
        + "," + std::to_string(info.capacity) + ");";
    executeSQLCommand(sql.c_str(), nullptr);
}

void WorldStorage::writeChunkToFile(std::fstream& file, const ChunkInfo& info,
    const Chunk::ChunkDataStorageType& data) {
    file.seekp(info.offset);
    file.write(reinterpret_cast<const char*>(data.data()), info.size);
}
