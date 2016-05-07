#ifndef WORLD_H_
#define WORLD_H_

#include <algorithm>
#include "chunk.h"

using ChunkID = unsigned long long;
const int worldBoundXLog2 = 26;
const int worldBoundYLog2 = 9;
const int worldBoundZLog2 = 26;
const int worldBoundX = 1 << worldBoundXLog2; // 2 ^ worldBoundXLog2
const int worldBoundY = 1 << worldBoundYLog2; // 2 ^ worldBoundYLog2
const int worldBoundZ = 1 << worldBoundZLog2; // 2 ^ worldBoundZLog2

class World
{
public:
    World() {};

    // Get chunk ID
    ChunkID getChunkID(Vec3 chunk_pos);
    // Get chunk pointer by index
    Chunk* getChunkPtr(int index);
    // Get chunk pointer by chunk coordinates
    Chunk* getChunkPtr(Vec3 chunk_pos);

private:
    // All chunks (chunk array)
    Chunk** chunks;
    // Size of chunk array
    int chunkSize;
    // Loaded chunks count
    int chunkCount;
    // Search chunk index
    int getChunkIndex(ChunkID chunk_id);
};

#endif // !WORLD_H_
