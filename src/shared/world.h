#ifndef WORLD_H_
#define WORLD_H_

#include <algorithm>
#include "chunk.h"

typedef unsigned long long ChunkID;
const int WORLD_BOUND_X_LOG2 = 26;
const int WORLD_BOUND_Y_LOG2 = 9;
const int WORLD_BOUND_Z_LOG2 = 26;
const int WORLD_BOUND_X = 1 << WORLD_BOUND_X_LOG2;
const int WORLD_BOUND_Y = 1 << WORLD_BOUND_Y_LOG2;
const int WORLD_BOUND_Z = 1 << WORLD_BOUND_Z_LOG2;

class World
{
    private:
        // All chunks (chunk array)
        Chunk** chunks;
        // Size of chunk array
        int chunksSize;
        // Loaded chunks count
        int chunkCount;

        // Search chunk index
        int getChunkIndex(ChunkID chunk_id);

    public:
        World();

        // Get chunk ID
        ChunkID getChunkID(Vec3 chunk_pos);
        // Get chunk pointer by index
        Chunk* getChunkPtr(int index);
        // Get chunk pointer by chunk coordinates
        Chunk* getChunkPtr(Vec3 chunk_pos);

};

#endif // !WORLD_H_
