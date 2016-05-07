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
public:
    World();

    // Get chunk pointer by index
    Chunk* getChunkPtr(int index);
    // Get chunk pointer by chunk coordinates
    Chunk* getChunkPtr(Vec3 chunkPos);

private:
    // All chunks (chunk array)
    Chunk** chunks;
    // Size of chunk array
    int chunkSize;
    // Loaded chunks count
    int chunkCount;
    // Search chunk index
    int getChunkIndex(ChunkID chunkID);
};

// Get chunk ID
ChunkID getChunkID(Vec3 chunkPos);

#endif // !WORLD_H_
