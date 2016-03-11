#pragma once
#include "Definitions.h"
#include "chunkPtrArray.h"
#include "HeightMap.h"
#include "Chunk.h"
#include "Hitbox.h"
#include "Blocks.h"
#include"Player.h"
#include"Particles.h"
#include"Items.h"

extern int viewdistance;
class Frsutum;

namespace World
{

void MarkBlockUpdate(Blocks::BUDDP Block);
extern string worldname;
const int worldsize = 134217728;
const int worldheight = 128;
extern brightness skylight;         //Sky light level
extern brightness BRIGHTNESSMAX;    //Maximum brightness
extern brightness BRIGHTNESSMIN;    //Mimimum brightness
extern brightness BRIGHTNESSDEC;    //Brightness decree
extern chunk* EmptyChunkPtr;
extern unsigned int EmptyBuffer;
extern int MaxChunkLoads;
extern int MaxChunkUnloads;
extern int MaxChunkRenders;

extern chunk** chunks;
extern int loadedChunks, chunkArraySize;
extern chunk* cpCachePtr;
extern chunkid cpCacheID;
extern HeightMap HMap;
extern chunkPtrArray cpArray;

extern int cloud[128][128];
extern int rebuiltChunks, rebuiltChunksCount;
extern int updatedChunks, updatedChunksCount;
extern int unloadedChunks, unloadedChunksCount;
extern int chunkBuildRenderList[256][2];
extern int chunkLoadList[256][4];
extern pair<chunk*, int> chunkUnloadList[256];
extern vector<unsigned int> vbuffersShouldDelete;
extern int chunkBuildRenders, chunkLoads, chunkUnloads;

void Init();

chunk* AddChunk(int x, int y, int z);
void DeleteChunk(int x, int y, int z);
inline chunkid getChunkID(int x, int y, int z)
{
    if (y == -128) y = 0;
    if (y <= 0) y = abs(y) + (1LL << 7);
    if (x == -134217728) x = 0;
    if (x <= 0) x = abs(x) + (1LL << 27);
    if (z == -134217728) z = 0;
    if (z <= 0) z = abs(z) + (1LL << 27);
    return (chunkid(y) << 56) + (chunkid(x) << 28) + z;
}
int getChunkPtrIndex(int x, int y, int z);
chunk* getChunkPtr(int x, int y, int z);
void ExpandChunkArray(int cc);
void ReduceChunkArray(int cc);

#define getchunkpos(n) ((n)>>4)
#define getblockpos(n) ((n)&15)
inline bool chunkOutOfBound(int x, int y, int z)
{
    return y < -World::worldheight || y > World::worldheight - 1 ||
           x < -134217728 || x > 134217727 || z < -134217728 || z > 134217727;
}
inline bool chunkLoaded(int x, int y, int z)
{
    if (chunkOutOfBound(x, y, z))return false;
    if (getChunkPtr(x, y, z) != nullptr)return true;
    return false;
}

vector<Hitbox::AABB> getHitboxes(const Hitbox::AABB& box);
bool inWater(const Hitbox::AABB& box);

void renderblock(int x, int y, int z, chunk* chunkptr);
void updateblock(int x, int y, int z, bool blockchanged, int depth = 0);
block& getblock(int x, int y, int z, block mask = block(Blocks::AIR), chunk* cptr = nullptr);
brightness getbrightness(int x, int y, int z, chunk* cptr = nullptr);
void setblock(int x, int y, int z, block Block, chunk* cptr = nullptr);
void setbrightness(int x, int y, int z, brightness Brightness, chunk* cptr = nullptr);
inline void putblock(int x, int y, int z, block Block)
{
    setblock(x, y, z, Block);
}
inline void pickleaf()
{
    if (rnd() < 0.05)
    {
        if (rnd() < 0.5)Player::addItem(APPLE);
        else Player::addItem(STICK);
    }
    else
    {
        Player::addItem(block(Blocks::LEAF));
    }
}
inline void picktree(int x, int y, int z)
{
    if (getblock(x, y, z) != block(Blocks::LEAF))Player::addItem(getblock(x, y, z));
    else pickleaf();
    for (int j = 1; j <=10; j++)
    {
        Particles::throwParticle(getblock(x, y, z),
                                 float(x + rnd() - 0.5f), float(y + rnd() - 0.2f), float(z + rnd() - 0.5f),
                                 float(rnd()*0.2f - 0.1f), float(rnd()*0.2f - 0.1f), float(rnd()*0.2f - 0.1f),
                                 float(rnd()*0.02 + 0.03), int(rnd() * 60) + 30);
    }
    setblock(x, y, z, block(Blocks::AIR));
    //上
    if ((getblock(x, y + 1, z) == block(Blocks::WOOD)) || (getblock(x, y + 1, z) == block(Blocks::LEAF)))picktree(x, y + 1, z);
    //前
    if ((getblock(x, y , z + 1) == block(Blocks::WOOD)) || (getblock(x, y , z + 1) == block(Blocks::LEAF)))picktree(x, y, z + 1);
    //后
    if ((getblock(x, y, z - 1) == block(Blocks::WOOD)) || (getblock(x, y, z - 1) == block(Blocks::LEAF)))picktree(x, y, z - 1);
    //左
    if ((getblock(x+1, y, z) == block(Blocks::WOOD)) || (getblock(x+1, y, z) == block(Blocks::LEAF)))picktree(x+1, y, z);
    //右
    if ((getblock(x - 1, y, z) == block(Blocks::WOOD)) || (getblock(x - 1, y, z) == block(Blocks::LEAF)))picktree(x - 1, y, z);
}
inline void pickblock(int x, int y, int z)
{
    if (getblock(x, y, z) == block(Blocks::WOOD) &&
            ((getblock(x, y+1, z) == block(Blocks::WOOD))|| (getblock(x, y + 1, z) == block(Blocks::LEAF))) &&
            (getblock(x, y, z + 1) == block(Blocks::AIR)) && (getblock(x, y, z - 1) == block(Blocks::AIR)) &&
            (getblock(x + 1, y, z) == block(Blocks::AIR)) && (getblock(x - 1, y, z) == block(Blocks::AIR)) &&
            ((getblock(x, y - 1, z) == block(Blocks::WOOD)) || (getblock(x, y - 1, z) == block(Blocks::DIRT)))
       )
    {
        picktree(x, y + 1, z);    //触发砍树模式
    }
    //击打树叶
    if (getblock(x, y, z)!=block(Blocks::LEAF))Player::addItem(getblock(x, y, z));
    else pickleaf();

    setblock(x, y, z, block(Blocks::AIR));
}


inline bool chunkInRange(int x, int y, int z, int px, int py, int pz, int dist)
{
    //检测给出的chunk坐标是否在渲染范围内
    if (x<px - dist || x>px + dist - 1 || y<py - dist || y>py + dist - 1 || z<pz - dist || z>pz + dist - 1) return false;
    return true;
}
bool chunkUpdated(int x, int y, int z);
void setChunkUpdated(int x, int y, int z, bool value);
void sortChunkBuildRenderList(int xpos, int ypos, int zpos);
void sortChunkLoadUnloadList(int xpos, int ypos, int zpos);
void calcVisible(double xpos, double ypos, double zpos, Frustum& frus);

void saveAllChunks();
void destroyAllChunks();

void buildtree(int x, int y, int z);
void explode(int x, int y, int z, int r, chunk* c = nullptr);
}
