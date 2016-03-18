#pragma once
#include "Definitions.h"
#include "Hitbox.h"
#include "Blocks.h"
#include "Frustum.h"
#include "World.h"

class Object;

namespace World
{

void MarkBlockUpdate(Blocks::BUDDP Block);
extern string worldname;
extern brightness BRIGHTNESSMIN;
extern brightness skylight;

class chunk;
chunkid getChunkID(int x, int y, int z);
void explode(int x, int y, int z, int r, chunk* c);

class chunk
{
private:
    brightness* pbrightness;
    vector<Object*> objects;
    static double relBaseX, relBaseY, relBaseZ;
    static Frustum TestFrustum;

public:
    block* pblocks; //moved here for blockupd would get block ptr directly
    //竟然一直都没有构造函数/析构函数 还要手动调用Init...我受不了啦(╯‵□′)╯︵┻━┻ --Null
    //2333 --qiaozhanrong
    chunk(int cxi, int cyi, int czi, chunkid idi) : cx(cxi), cy(cyi), cz(czi), id(idi),
        Modified(false), Empty(false), updated(false), renderBuilt(false), DetailGenerated(false), loadAnim(0.0)
    {
        memset(vertexes, 0, sizeof(vertexes));
        memset(vbuffer, 0, sizeof(vbuffer));
    }

    int cx, cy, cz;
    Hitbox::AABB aabb;
    bool Empty, updated, renderBuilt, Modified, DetailGenerated;
    chunkid id;
    vtxCount vertexes[4];
    VBOID vbuffer[4];
    double loadAnim;
    bool visible;

    void create();
    void destroy();
    void Load(bool initIfEmpty = true);
    void Unload();
    void buildTerrain(bool initIfEmpty = true);
    void buildDetail();
    void build(bool initIfEmpty = true);

    string getChunkPath()
    {
        std::stringstream ss;
        ss << "Worlds/" << worldname << "/chunks/chunk_" << cx << "_" << cy << "_" << cz << ".NEWorldChunk";
        return ss.str();
    }

    string getObjectsPath()
    {
        std::stringstream ss;
        ss << "Worlds/" << worldname << "/objects/chunk_" << cx << "_" << cy << "_" << cz << ".NEWorldObjects";
        return ss.str();
    }

    bool fileExist(string path)
    {
        std::fstream file;
        file.open(path, std::ios::in);
        bool ret = file.is_open();
        file.close();
        return ret;
    }

    bool LoadFromFile(); //返回true代表区块文件打开成功
    void SaveToFile();
    void buildRender();
    void destroyRender();

    inline block& getblock(int x, int y, int z)
    {
#ifdef NEWORLD_DEBUG_CONSOLE_OUTPUT
        if (pblocks == nullptr)
        {
            DebugWarning("chunk.getblock() error: Empty pointer");
            return;
        }
        if (x>15 || x<0 || y>15 || y<0 || z>15 || z<0)
        {
            DebugWarning("chunk.getblock() error: Out of range");
            return;
        }
#endif
        return pblocks[(x << 8) ^ (y << 4) ^ z];
    }

    brightness getbrightness(int x, int y, int z)
    {
#ifdef NEWORLD_DEBUG_CONSOLE_OUTPUT
        if (pbrightness == nullptr)
        {
            DebugWarning("chunk.getbrightness() error: Empty pointer");
            return;
        }
        if (x>15 || x<0 || y>15 || y<0 || z>15 || z<0)
        {
            DebugWarning("chunk.getbrightness() error: Out of range");
            return;
        }
#endif
        return pbrightness[(x << 8) ^ (y << 4) ^ z];
    }

    void setblock(int x, int y, int z, block iblock)
    {
        if (iblock == block(Blocks::TNT))
        {
            World::explode(cx * 16 + x, cy * 16 + y, cz * 16 + z, 8, this);
            return;
        }
        pblocks[(x << 8) ^ (y << 4) ^ z] = iblock;
        Modified = true;
    }

    void Modifyblock(int x, int y, int z, block iblock)
    {
        if (iblock == block(Blocks::TNT))
        {
            World::explode(cx * 16 + x, cy * 16 + y, cz * 16 + z, 8, this);
            return;
        }
		MarkBlockUpdate(Blocks::BUDDP(*(pblocks + ((x << 8) ^ (y << 4) ^ z)), pblocks + ((x << 8) ^ (y << 4) ^ z), nullptr, nullptr, nullptr, cx * 16 + x, cy * 16 + y, cz * 16 + z));
        pblocks[(x << 8) ^ (y << 4) ^ z] = iblock;
		Modified = true;
    }

    inline void setbrightness(int x, int y, int z, brightness ibrightness)
    {
        pbrightness[(x << 8) ^ (y << 4) ^ z] = ibrightness;
        Modified = true;
    }

    static void setRelativeBase(double x, double y, double z, Frustum& frus)
    {
        relBaseX = x;
        relBaseY = y;
        relBaseZ = z;
        TestFrustum = frus;
    }

    Hitbox::AABB getBaseAABB();
    Frustum::ChunkBox getRelativeAABB();
    inline void calcVisible()
    {
        visible = TestFrustum.FrustumTest(getRelativeAABB());
    }

};
}
