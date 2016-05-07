#ifndef WORLD_H_
#define WORLD_H_

#include <algorithm>
#include "common.h"
#include "chunk.h"

const int WorldBoundXLog2 = 26;
const int WorldBoundYLog2 = 9;
const int WorldBoundZLog2 = 26;
const int WorldBoundX = 1 << WorldBoundXLog2; // 2 ^ WorldBoundXLog2
const int WorldBoundY = 1 << WorldBoundYLog2; // 2 ^ WorldBoundYLog2
const int WorldBoundZ = 1 << WorldBoundZLog2; // 2 ^ WorldBoundZLog2

class World
{
private:
    // All chunks (chunk array)
    Chunk** chunks;
    // Size of chunk array
    int chunkSize;
    // Loaded chunks count
    int chunkCount;

    // Get chunk ID
    ChunkID getChunkID(Vec3 chunkPos);
    // Search chunk index, or the index the chunk should insert into
    int getChunkIndex(ChunkID chunkID);

public:
    // Initializes world
    World();

    // Add chunk
    Chunk* AddChunk(Vec3 chunkPos);
    // Delete chunk
    int DeleteChunk(Vec3 chunkPos);
    // Get chunk pointer by index
    Chunk* getChunkPtr(int index);
    // Get chunk pointer by chunk coordinates
    Chunk* getChunkPtr(Vec3 chunkPos);
    // Convert world position to chunk coordinate (one axis)
    int getChunkPos(int pos) { return pos >> ChunkSizeLog2; }
    // Convert world position to chunk coordinate (all axes)
    Vec3 getChunkPos(Vec3 pos) { return Vec3(getChunkPos(pos.x), getChunkPos(pos.y), getChunkPos(pos.z)); }
    // Convert world position to block coordinate in chunk (one axis)
    int getBlockPos(int pos) { return pos & (ChunkSize - 1); }
    // Convert world position to block coordinate in chunk (all axes)
    Vec3 getBlockPos(Vec3 pos) { return Vec3(getBlockPos(pos.x), getBlockPos(pos.y), getBlockPos(pos.z)); }
    // Get block data
    BlockData getBlock(Vec3 pos);
    // Set block data
    void setBlock(Vec3 pos, BlockData block);

};

#endif // !WORLD_H_
