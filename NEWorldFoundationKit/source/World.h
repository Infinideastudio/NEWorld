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
void ProcessBuq();
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
chunkid getChunkID(int x, int y, int z);
int getChunkPtrIndex(int x, int y, int z);
chunk* getChunkPtr(int x, int y, int z);
void ExpandChunkArray(int cc);
void ReduceChunkArray(int cc);

#define getchunkpos(n) ((n)>>4)
#define getblockpos(n) ((n)&15)

inline bool chunkOutOfBound(int x, int y, int z);
inline bool chunkLoaded(int x, int y, int z);
vector<Hitbox::AABB> getHitboxes(const Hitbox::AABB& box);
bool inWater(const Hitbox::AABB& box);
void renderblock(int x, int y, int z, chunk* chunkptr);
void updateblock(int x, int y, int z, bool blockchanged, int depth = 0);
block getblock(int x, int y, int z, block mask = block(Blocks::AIR), chunk* cptr = nullptr);
brightness getbrightness(int x, int y, int z, chunk* cptr = nullptr);
void setblock(int x, int y, int z, block Block, chunk* cptr = nullptr);
void Modifyblock(int x, int y, int z, block Block, chunk* cptr = nullptr);
void setbrightness(int x, int y, int z, brightness Brightness, chunk* cptr = nullptr);
void putblock(int x, int y, int z, block Block);
void pickleaf();
void picktree(int x, int y, int z);
void pickblock(int x, int y, int z);
bool chunkInRange(int x, int y, int z, int px, int py, int pz, int dist);
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
