#ifndef WORLDLOADER_H_
#define WORLDLOADER_H_

#include <utility>
#include "world.h"
using std::pair;

const int MaxChunkLoadCount = 64;
const int MaxChunkUnloadCount = 64;

class WorldLoader
{
    private:
        World* _world;
        int _chunkLoadCount, _chunkUnloadCount;
        pair<Vec3, int> _chunkLoadList[256]; // Chunk load list <position, distance>
        pair<Chunk*, int> _chunkUnloadList[256]; // Chunk unload list <pointer, distance>

    public:
        WorldLoader(World* _world_) :_world(_world_) {}

        // Find the nearest chunks in load distance to load, fartherest chunks out of load distance to unload
        void sortChunkLoadUnloadList(const Vec3& centerPos);

};

#endif // !WORLDLOADER_H_
