#include "World.h"
#include "Textures.h"
#include "Renderer.h"
#include "WorldGen.h"
#include "Particles.h"

namespace World
{

string worldname;
brightness skylight = 15;         //Sky light level
brightness BRIGHTNESSMAX = 15;    //Maximum brightness
brightness BRIGHTNESSMIN = 2;     //Mimimum brightness
brightness BRIGHTNESSDEC = 1;     //Brightness decrease
chunk* EmptyChunkPtr = (chunk*)~0;
unsigned int EmptyBuffer;
int MaxChunkLoads = 64;
int MaxChunkUnloads = 64;
int MaxChunkRenders = 1;

vector<chunk*> chunks;
chunk* cpCachePtr = nullptr;
chunkid cpCacheID = 0;
chunkPtrArray cpArray;
HeightMap HMap;
int cloud[128][128];
int rebuiltChunks, rebuiltChunksCount;
int updatedChunks, updatedChunksCount;
int unloadedChunks, unloadedChunksCount;
int chunkBuildRenderList[256][2];
int chunkLoadList[256][4];
pair<chunk*, int> chunkUnloadList[256];
vector<unsigned int> vbuffersShouldDelete;
int chunkBuildRenders, chunkLoads, chunkUnloads;
//bool* loadedChunkArray = nullptr; //Accelerate sortings

void Init()
{
    _mkdir(("Worlds/" + worldname + "/").c_str());
    _mkdir(("Worlds/" + worldname + "/chunks").c_str());

    WorldGen::perlinNoiseInit(3404);
    cpCachePtr = nullptr;
    cpCacheID = 0;

    HMap.setSize((ViewDistance + 2) * 2 * 16);
    HMap.create();
}

inline pair<int,int> binary_search_chunks(chunkid cid)
{
    int first = 0;
    int last = chunks.size() - 1;
    int middle = (first + last) / 2;
    while (first <= last && chunks[middle]->id != cid)
    {
        if (chunks[middle]->id > cid)
        {
            last = middle - 1;
        }
        if (chunks[middle]->id < cid)
        {
            first = middle + 1;
        }
        middle = (first + last) / 2;
    }
    return std::make_pair(first, middle);
}

chunk* AddChunk(int x, int y, int z)
{
    chunkid cid;
    cid = getChunkID(x, y, z);  //Chunk ID
    pair<int, int> pos = binary_search_chunks(cid);
    if (chunks.size() && chunks[pos.second]->id == cid)
    {
        printf("[Console][Error]");
        printf("Chunk(%d,%d,%d)has been loaded,when adding chunk.\n", x, y, z);
        return chunks[pos.second];
    }

	chunks.push_back(nullptr);
    for (int i = chunks.size() - 1; i >= pos.first + 1; i--)
    {
        chunks[i] = chunks[i - 1];
    }
    chunks[pos.first] = new chunk(x, y, z, cid);
    cpCacheID = cid;
    cpCachePtr = chunks[pos.first];
    cpArray.AddChunk(chunks[pos.first],x,y,z);
    return chunks[pos.first];
}

void DeleteChunk(int x, int y, int z)
{
    int index = World::getChunkPtrIndex(x, y, z);
    if (cpCachePtr == chunks[index])
    {
        cpCacheID = 0;
        cpCachePtr = nullptr;
    }
    delete chunks[index];
	chunks.erase(chunks.begin() + index);
    cpArray.DeleteChunk(x, y, z);
}

int getChunkPtrIndex(int x, int y, int z)
{
#ifdef NEWORLD_DEBUG_PERFORMANCE_REC
    c_getChunkPtrFromSearch++;
#endif
    chunkid cid = getChunkID(x, y, z);
    pair<int, int> pos = binary_search_chunks(cid);
    if (chunks[pos.second]->id == cid) return pos.second;
	assert(false);
    return -1;
}

chunk* getChunkPtr(int x, int y, int z)
{
    chunkid cid = getChunkID(x, y, z);
    if (cpCacheID == cid && cpCachePtr != nullptr) return cpCachePtr;
    else
    {
        chunk* ret = cpArray.getChunkPtr(x, y, z);
        if (ret != nullptr)
        {
            cpCacheID = cid;
            cpCachePtr = ret;
            return ret;
        }
        if (chunks.size())
        {
#ifdef NEWORLD_DEBUG_PERFORMANCE_REC
            c_getChunkPtrFromSearch++;
#endif
            pair<int, int> pos = binary_search_chunks(cid);
            if (chunks[pos.second]->id == cid)
            {
                ret = chunks[pos.second];
                cpCacheID = cid;
                cpCachePtr = ret;
                if (cpArray.elementExists(x - cpArray.originX, y - cpArray.originY, z - cpArray.originZ))
                {
                    cpArray.array[(x - cpArray.originX)*cpArray.size2 + (y - cpArray.originY)*cpArray.size + (z - cpArray.originZ)] = chunks[pos.second];
                }
                return ret;
            }
        }
    }
    return nullptr;
}

void renderblock(int x, int y, int z, chunk* chunkptr)
{

    double colors, color1, color2, color3, color4, tcx, tcy, size, EPS = 0.0;
    int cx = chunkptr->cx, cy = chunkptr->cy, cz = chunkptr->cz;
    int gx = cx * 16 + x, gy = cy * 16 + y, gz = cz * 16 + z;
    block blk[7] = { chunkptr->getblock(x,y,z) ,
                     z < 15 ? chunkptr->getblock(x, y, z + 1) : getblock(gx, gy, gz + 1, block(Blocks::ROCK)),
                     z>0 ? chunkptr->getblock(x, y, z - 1) : getblock(gx, gy, gz - 1, block(Blocks::ROCK)),
                     x < 15 ? chunkptr->getblock(x + 1, y, z) : getblock(gx + 1, gy, gz, block(Blocks::ROCK)),
                     x>0 ? chunkptr->getblock(x - 1, y, z) : getblock(gx - 1, gy, gz, block(Blocks::ROCK)),
                     y < 15 ? chunkptr->getblock(x, y + 1, z) : getblock(gx, gy + 1, gz, block(Blocks::ROCK)),
                     y>0 ? chunkptr->getblock(x, y - 1, z) : getblock(gx, gy - 1, gz, block(Blocks::ROCK))
                   };

    brightness brt[7] = { chunkptr->getbrightness(x,y,z) ,
                          z < 15 ? chunkptr->getbrightness(x,y,z + 1) : getbrightness(gx,gy,gz + 1),
                          z>0 ? chunkptr->getbrightness(x,y,z - 1) : getbrightness(gx,gy,gz - 1),
                          x < 15 ? chunkptr->getbrightness(x + 1,y,z) : getbrightness(gx + 1,gy,gz),
                          x>0 ? chunkptr->getbrightness(x - 1,y,z) : getbrightness(gx - 1,gy,gz),
                          y < 15 ? chunkptr->getbrightness(x,y + 1,z) : getbrightness(gx,gy + 1,gz),
                          y>0 ? chunkptr->getbrightness(x,y - 1,z) : getbrightness(gx,gy - 1,gz)
                        };

    size = 1 / 8.0f - EPS;

    if (NiceGrass && blk[0] == block(Blocks::GRASS) && getblock(gx, gy - 1, gz + 1, block(Blocks::ROCK), chunkptr) == block(Blocks::GRASS))
    {
        tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
    }
    else
    {
        tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
    }

    // Front Face
    if (!(BlockInfo(blk[1]).isOpaque() || (blk[1] == blk[0] && BlockInfo(blk[0]).isOpaque() == false)) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[1];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (colors + getbrightness(gx, gy - 1, gz + 1) + getbrightness(gx - 1, gy, gz + 1) + getbrightness(gx - 1, gy - 1, gz + 1)) / 4.0;
            color2 = (colors + getbrightness(gx, gy - 1, gz + 1) + getbrightness(gx + 1, gy, gz + 1) + getbrightness(gx + 1, gy - 1, gz + 1)) / 4.0;
            color3 = (colors + getbrightness(gx, gy + 1, gz + 1) + getbrightness(gx + 1, gy, gz + 1) + getbrightness(gx + 1, gy + 1, gz + 1)) / 4.0;
            color4 = (colors + getbrightness(gx, gy + 1, gz + 1) + getbrightness(gx - 1, gy, gz + 1) + getbrightness(gx - 1, gy + 1, gz + 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;
        if (blk[0] != block(Blocks::GLOWSTONE) && !Renderer::AdvancedRender)
        {
            color1 *= 0.5;
            color2 *= 0.5;
            color3 *= 0.5;
            color4 *= 0.5;
        }

        if (Renderer::AdvancedRender) Renderer::Attrib1f(0.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx, tcy);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, 0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size, tcy);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, 0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size, tcy + size);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, 0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx, tcy + size);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, 0.5 + z);

    }

    if (NiceGrass && blk[0] == block(Blocks::GRASS) && getblock(gx, gy - 1, gz - 1, block(Blocks::ROCK), chunkptr) == block(Blocks::GRASS))
    {
        tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
    }
    else
    {
        tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
    }

    // Back Face
    if (!(BlockInfo(blk[2]).isOpaque() || (blk[2] == blk[0] && BlockInfo(blk[0]).isOpaque() == false)) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[2];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (colors + getbrightness(gx, gy - 1, gz - 1) + getbrightness(gx - 1, gy, gz - 1) + getbrightness(gx - 1, gy - 1, gz - 1)) / 4.0;
            color2 = (colors + getbrightness(gx, gy + 1, gz - 1) + getbrightness(gx - 1, gy, gz - 1) + getbrightness(gx - 1, gy + 1, gz - 1)) / 4.0;
            color3 = (colors + getbrightness(gx, gy + 1, gz - 1) + getbrightness(gx + 1, gy, gz - 1) + getbrightness(gx + 1, gy + 1, gz - 1)) / 4.0;
            color4 = (colors + getbrightness(gx, gy - 1, gz - 1) + getbrightness(gx + 1, gy, gz - 1) + getbrightness(gx + 1, gy - 1, gz - 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;
        if (blk[0] != block(Blocks::GLOWSTONE) && !Renderer::AdvancedRender)
        {
            color1 *= 0.5;
            color2 *= 0.5;
            color3 *= 0.5;
            color4 *= 0.5;
        }

        if (Renderer::AdvancedRender) Renderer::Attrib1f(1.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*0.0);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, -0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*1.0);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, -0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*1.0);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, -0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*0.0);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, -0.5 + z);

    }

    if (NiceGrass && blk[0] == block(Blocks::GRASS) && getblock(gx + 1, gy - 1, gz, block(Blocks::ROCK), chunkptr) == block(Blocks::GRASS))
    {
        tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
    }
    else
    {
        tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
    }

    // Right face
    if (!(BlockInfo(blk[3]).isOpaque() || (blk[3] == blk[0] && !BlockInfo(blk[0]).isOpaque())) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[3];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (colors + getbrightness(gx + 1, gy - 1, gz) + getbrightness(gx + 1, gy, gz - 1) + getbrightness(gx + 1, gy - 1, gz - 1)) / 4.0;
            color2 = (colors + getbrightness(gx + 1, gy + 1, gz) + getbrightness(gx + 1, gy, gz - 1) + getbrightness(gx + 1, gy + 1, gz - 1)) / 4.0;
            color3 = (colors + getbrightness(gx + 1, gy + 1, gz) + getbrightness(gx + 1, gy, gz + 1) + getbrightness(gx + 1, gy + 1, gz + 1)) / 4.0;
            color4 = (colors + getbrightness(gx + 1, gy - 1, gz) + getbrightness(gx + 1, gy, gz + 1) + getbrightness(gx + 1, gy - 1, gz + 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;
        if (blk[0] != block(Blocks::GLOWSTONE) && !Renderer::AdvancedRender)
        {
            color1 *= 0.7;
            color2 *= 0.7;
            color3 *= 0.7;
            color4 *= 0.7;
        }

        if (Renderer::AdvancedRender) Renderer::Attrib1f(2.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*0.0);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, -0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*1.0);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, -0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*1.0);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, 0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*0.0);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, 0.5 + z);

    }

    if (NiceGrass && blk[0] == block(Blocks::GRASS) && getblock(gx - 1, gy - 1, gz, block(Blocks::ROCK), chunkptr) == block(Blocks::GRASS))
    {
        tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
    }
    else
    {
        tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
    }

    // Left Face
    if (!(BlockInfo(blk[4]).isOpaque() || (blk[4] == blk[0] && BlockInfo(blk[0]).isOpaque() == false)) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[4];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (colors + getbrightness(gx - 1, gy - 1, gz) + getbrightness(gx - 1, gy, gz - 1) + getbrightness(gx - 1, gy - 1, gz - 1)) / 4.0;
            color2 = (colors + getbrightness(gx - 1, gy - 1, gz) + getbrightness(gx - 1, gy, gz + 1) + getbrightness(gx - 1, gy - 1, gz + 1)) / 4.0;
            color3 = (colors + getbrightness(gx - 1, gy + 1, gz) + getbrightness(gx - 1, gy, gz + 1) + getbrightness(gx - 1, gy + 1, gz + 1)) / 4.0;
            color4 = (colors + getbrightness(gx - 1, gy + 1, gz) + getbrightness(gx - 1, gy, gz - 1) + getbrightness(gx - 1, gy + 1, gz - 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;
        if (blk[0] != block(Blocks::GLOWSTONE) && !Renderer::AdvancedRender)
        {
            color1 *= 0.7;
            color2 *= 0.7;
            color3 *= 0.7;
            color4 *= 0.7;
        }

        if (Renderer::AdvancedRender) Renderer::Attrib1f(3.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*0.0);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, -0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*0.0);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, 0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*1.0);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, 0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*1.0);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, -0.5 + z);

    }

    tcx = Textures::getTexcoordX(blk[0], 1);
    tcy = Textures::getTexcoordY(blk[0], 1);

    // Top Face
    if (!(BlockInfo(blk[5]).isOpaque() || (blk[5] == blk[0] && BlockInfo(blk[0]).isOpaque() == false)) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[5];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (color1 + getbrightness(gx, gy + 1, gz - 1) + getbrightness(gx - 1, gy + 1, gz) + getbrightness(gx - 1, gy + 1, gz - 1)) / 4.0;
            color2 = (color2 + getbrightness(gx, gy + 1, gz + 1) + getbrightness(gx - 1, gy + 1, gz) + getbrightness(gx - 1, gy + 1, gz + 1)) / 4.0;
            color3 = (color3 + getbrightness(gx, gy + 1, gz + 1) + getbrightness(gx + 1, gy + 1, gz) + getbrightness(gx + 1, gy + 1, gz + 1)) / 4.0;
            color4 = (color4 + getbrightness(gx, gy + 1, gz - 1) + getbrightness(gx + 1, gy + 1, gz) + getbrightness(gx + 1, gy + 1, gz - 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;

        if (Renderer::AdvancedRender) Renderer::Attrib1f(4.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*1.0);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, -0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*0.0);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, 0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*0.0);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, 0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*1.0);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, -0.5 + z);

    }

    tcx = Textures::getTexcoordX(blk[0], 3);
    tcy = Textures::getTexcoordY(blk[0], 3);

    // Bottom Face
    if (!(BlockInfo(blk[6]).isOpaque() || (blk[6] == blk[0] && BlockInfo(blk[0]).isOpaque() == false)) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[6];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (colors + getbrightness(gx, gy - 1, gz - 1) + getbrightness(gx - 1, gy - 1, gz) + getbrightness(gx - 1, gy - 1, gz - 1)) / 4.0;
            color2 = (colors + getbrightness(gx, gy - 1, gz - 1) + getbrightness(gx + 1, gy - 1, gz) + getbrightness(gx + 1, gy - 1, gz - 1)) / 4.0;
            color3 = (colors + getbrightness(gx, gy - 1, gz + 1) + getbrightness(gx + 1, gy - 1, gz) + getbrightness(gx + 1, gy - 1, gz + 1)) / 4.0;
            color4 = (colors + getbrightness(gx, gy - 1, gz + 1) + getbrightness(gx - 1, gy - 1, gz) + getbrightness(gx - 1, gy - 1, gz + 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;

        if (Renderer::AdvancedRender) Renderer::Attrib1f(5.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*1.0);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, -0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*1.0);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, -0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*0.0);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, 0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*0.0);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, 0.5 + z);

    }
}

vector<Hitbox::AABB> getHitboxes(const Hitbox::AABB& box)
{
    //返回与box相交的所有方块AABB

    Hitbox::AABB blockbox;
    vector<Hitbox::AABB> Hitboxes;

    for (int a = int(box.xmin + 0.5) - 1; a <= int(box.xmax + 0.5) + 1; a++)
    {
        for (int b = int(box.ymin + 0.5) - 1; b <= int(box.ymax + 0.5) + 1; b++)
        {
            for (int c = int(box.zmin + 0.5) - 1; c <= int(box.zmax + 0.5) + 1; c++)
            {
                if (BlockInfo(getblock(a, b, c)).isSolid())
                {
                    blockbox.xmin = a - 0.5;
                    blockbox.xmax = a + 0.5;
                    blockbox.ymin = b - 0.5;
                    blockbox.ymax = b + 0.5;
                    blockbox.zmin = c - 0.5;
                    blockbox.zmax = c + 0.5;
                    if (Hitbox::Hit(box, blockbox)) Hitboxes.push_back(blockbox);
                }
            }
        }
    }
    return Hitboxes;
}

bool inWater(const Hitbox::AABB& box)
{
    Hitbox::AABB blockbox;
    for (int a = int(box.xmin + 0.5) - 1; a <= int(box.xmax + 0.5); a++)
    {
        for (int b = int(box.ymin + 0.5) - 1; b <= int(box.ymax + 0.5); b++)
        {
            for (int c = int(box.zmin + 0.5) - 1; c <= int(box.zmax + 0.5); c++)
            {
                if (getblock(a, b, c) == block(Blocks::WATER) || getblock(a, b, c) == block(Blocks::LAVA))
                {
                    blockbox.xmin = a - 0.5;
                    blockbox.xmax = a + 0.5;
                    blockbox.ymin = b - 0.5;
                    blockbox.ymax = b + 0.5;
                    blockbox.zmin = c - 0.5;
                    blockbox.zmax = c + 0.5;
                    if (Hitbox::Hit(box, blockbox)) return true;
                }
            }
        }
    }
    return false;
}

void updateblock(int x, int y, int z, bool blockchanged, int depth)
{
    //Blockupdate

    if (depth > 4096) return;
    depth++;

    bool updated = blockchanged;
    int cx = getchunkpos(x);
    int cy = getchunkpos(y);
    int cz = getchunkpos(z);

    if (chunkOutOfBound(cx, cy, cz)) return;

    int bx = getblockpos(x);
    int by = getblockpos(y);
    int bz = getblockpos(z);

    chunk* cptr = getChunkPtr(cx, cy, cz);
    if (cptr != nullptr)
    {

        if (cptr == EmptyChunkPtr)
        {
            cptr = World::AddChunk(cx, cy, cz);
            cptr->Load();
            cptr->Empty = false;
        }
        brightness oldbrightness = cptr->getbrightness(bx, by, bz);
        bool skylighted = true;
        int yi, cyi;
        yi = y + 1;
        cyi = getchunkpos(yi);
        if (y < 0) skylighted = false;
        else
        {
            while (!chunkOutOfBound(cx, cyi + 1, cz) && chunkLoaded(cx, cyi + 1, cz) && skylighted)
            {
                if (BlockInfo(getblock(x, yi, z)).isOpaque() || getblock(x, yi, z) == block(Blocks::WATER))
                {
                    skylighted = false;
                }
                yi++;
                cyi = getchunkpos(yi);
            }
        }

        if (!BlockInfo(getblock(x, y, z)).isOpaque())
        {

            brightness br;
            int maxbrightness;
            block blks[7] = { 0,
                              getblock(x, y, z + 1),    //Front face
                              getblock(x, y, z - 1),    //Back face
                              getblock(x + 1, y, z),    //Right face
                              getblock(x - 1, y, z),    //Left face
                              getblock(x, y + 1, z),    //Top face
                              getblock(x, y - 1, z)
                            };  //Bottom face
            brightness brts[7] = { 0,
                                   getbrightness(x, y, z + 1),    //Front face
                                   getbrightness(x, y, z - 1),    //Back face
                                   getbrightness(x + 1, y, z),    //Right face
                                   getbrightness(x - 1, y, z),    //Left face
                                   getbrightness(x, y + 1, z),    //Top face
                                   getbrightness(x, y - 1, z)
                                 };  //Bottom face
            maxbrightness = 1;
            for (int i = 2; i <= 6; i++)
            {
                if (brts[maxbrightness] < brts[i]) maxbrightness = i;
            }
            br = brts[maxbrightness];
            if (blks[maxbrightness] == block(Blocks::WATER))
            {
                if (br - 2 < BRIGHTNESSMIN) br = BRIGHTNESSMIN;
                else br -= 2;
            }
            else
            {
                if (br - 1 < BRIGHTNESSMIN) br = BRIGHTNESSMIN;
                else br--;
            }

            if (skylighted)
            {
                if (br < skylight) br = skylight;
            }
            if (br < BRIGHTNESSMIN) br = BRIGHTNESSMIN;
            //Set brightness
            cptr->setbrightness(bx, by, bz, br);

        }
        else
        {

            //Opaque block
            cptr->setbrightness(bx, by, bz, 0);
            if (getblock(x, y, z) == block(Blocks::GLOWSTONE) || getblock(x, y, z) == block(Blocks::LAVA))
            {
                cptr->setbrightness(bx, by, bz, BRIGHTNESSMAX);
            }

        }

        if (oldbrightness != cptr->getbrightness(bx, by, bz)) updated = true;

        if (updated)
        {
            updateblock(x, y + 1, z, false, depth);
            updateblock(x, y - 1, z, false, depth);
            updateblock(x + 1, y, z, false, depth);
            updateblock(x - 1, y, z, false, depth);
            updateblock(x, y, z + 1, false, depth);
            updateblock(x, y, z - 1, false, depth);
        }

        setChunkUpdated(cx, cy, cz, true);
        if (bx == 15 && cx < worldsize - 1) setChunkUpdated(cx + 1, cy, cz, true);
        if (bx == 0 && cx > -worldsize) setChunkUpdated(cx - 1, cy, cz, true);
        if (by == 15 && cy < worldheight - 1) setChunkUpdated(cx, cy + 1, cz, true);
        if (by == 0 && cy > -worldheight) setChunkUpdated(cx, cy - 1, cz, true);
        if (bz == 15 && cz < worldsize - 1) setChunkUpdated(cx, cy, cz + 1, true);
        if (bz == 0 && cz > -worldsize) setChunkUpdated(cx, cy, cz - 1, true);

    }
}

void Modifyblock(int x, int y, int z, block Blockname, chunk* cptr)
{
    //设置方块
    int	cx = getchunkpos(x), cy = getchunkpos(y), cz = getchunkpos(z);
    int bx = getblockpos(x), by = getblockpos(y), bz = getblockpos(z);

    if (cptr != nullptr && cptr != EmptyChunkPtr &&
            cx == cptr->cx && cy == cptr->cy && cz == cptr->cz)
    {
        cptr->Modifyblock(bx, by, bz, Blockname);
        updateblock(x, y, z, true);
    }
    if (!chunkOutOfBound(cx, cy, cz))
    {
        chunk* i = getChunkPtr(cx, cy, cz);
        if (i == EmptyChunkPtr)
        {
            chunk* cp = AddChunk(cx, cy, cz);
            cp->Load();
            cp->Empty = false;
            i = cp;
        }
        if (i != nullptr)
        {
            i->Modifyblock(bx, by, bz, Blockname);
            updateblock(x, y, z, true);
        }
    }
}

block getblock(int x, int y, int z, block mask, chunk* cptr)
{
    //获取方块
    int    cx = getchunkpos(x), cy = getchunkpos(y), cz = getchunkpos(z);
    if (chunkOutOfBound(cx, cy, cz)) return block(Blocks::AIR);
    int bx = getblockpos(x), by = getblockpos(y), bz = getblockpos(z);
    if (cptr != nullptr && cx == cptr->cx && cy == cptr->cy && cz == cptr->cz)
    {
        return cptr->getblock(bx, by, bz);
    }
    chunk* ci = getChunkPtr(cx, cy, cz);
    if (ci == EmptyChunkPtr) return block(Blocks::AIR);
    if (ci != nullptr) return ci->getblock(bx, by, bz);
    return mask;
}

brightness getbrightness(int x, int y, int z, chunk* cptr)
{
    //获取亮度
    int cx = getchunkpos(x), cy = getchunkpos(y), cz = getchunkpos(z);
    if (chunkOutOfBound(cx, cy, cz)) return skylight;
    int bx = getblockpos(x), by = getblockpos(y), bz = getblockpos(z);
    if (cptr != nullptr && cx == cptr->cx && cy == cptr->cy && cz == cptr->cz)
    {
        return cptr->getbrightness(bx, by, bz);
    }
    chunk* ci = getChunkPtr(cx, cy, cz);
    if (ci == EmptyChunkPtr) if (cy < 0) return BRIGHTNESSMIN;
        else return skylight;
    if (ci != nullptr)return ci->getbrightness(bx, by, bz);
    return skylight;
}

void setblock(int x, int y, int z, block Blockname, chunk* cptr)
{
    //设置方块
    int cx = getchunkpos(x), cy = getchunkpos(y), cz = getchunkpos(z);
    int bx = getblockpos(x), by = getblockpos(y), bz = getblockpos(z);

    if (cptr != nullptr && cptr != EmptyChunkPtr &&
            cx == cptr->cx && cy == cptr->cy && cz == cptr->cz)
    {
        cptr->setblock(bx, by, bz, Blockname);
        updateblock(x, y, z, true);
    }
    if (!chunkOutOfBound(cx, cy, cz))
    {
        chunk* i = getChunkPtr(cx, cy, cz);
        if (i == EmptyChunkPtr)
        {
            chunk* cp = AddChunk(cx, cy, cz);
            cp->Load();
            cp->Empty = false;
            i = cp;
        }
        if (i != nullptr)
        {
            i->setblock(bx, by, bz, Blockname);
            updateblock(x, y, z, true);
        }
    }
}

void setbrightness(int x, int y, int z, brightness Brightness, chunk* cptr)
{
    //设置亮度
    int    cx = getchunkpos(x), cy = getchunkpos(y), cz = getchunkpos(z);
    int bx = getblockpos(x), by = getblockpos(y), bz = getblockpos(z);

    if (cptr != nullptr && cptr != EmptyChunkPtr &&
            cx == cptr->cx && cy == cptr->cy && cz == cptr->cz)
    {
        cptr->setbrightness(bx, by, bz, Brightness);
    }
    if (!chunkOutOfBound(cx, cy, cz))
    {
        chunk* i = getChunkPtr(cx, cy, cz);
        if (i == EmptyChunkPtr)
        {
            chunk* cp = AddChunk(cx, cy, cz);
            cp->Load();
            cp->Empty = false;
            i = cp;
        }
        if (i != nullptr)
        {
            i->setbrightness(bx, by, bz, Brightness);
        }
    }
}

bool chunkUpdated(int x, int y, int z)
{
    chunk* i = getChunkPtr(x, y, z);
    if (i == nullptr || i == EmptyChunkPtr) return false;
    return i->updated;
}

void setChunkUpdated(int x, int y, int z, bool value)
{
    chunk* i = getChunkPtr(x, y, z);
    if (i == EmptyChunkPtr)
    {
        chunk* cp = AddChunk(x, y, z);
        cp->Load();
        cp->Empty = false;
        i = cp;
    }
    if (i != nullptr) i->updated = value;
}

void sortChunkBuildRenderList(int xpos, int ypos, int zpos)
{
    int cxp, cyp, czp, cx, cy, cz, p = 0;
    int xd, yd, zd, distsqr;

    cxp = getchunkpos(xpos);
    cyp = getchunkpos(ypos);
    czp = getchunkpos(zpos);

    for (size_t ci = 0; ci < chunks.size(); ci++)
    {
        if (chunks[ci]->updated)
        {
            cx = chunks[ci]->cx;
            cy = chunks[ci]->cy;
            cz = chunks[ci]->cz;
            if (!chunkInRange(cx, cy, cz, cxp, cyp, czp, ViewDistance)) continue;
            xd = cx * 16 + 7 - xpos;
            yd = cy * 16 + 7 - ypos;
            zd = cz * 16 + 7 - zpos;
            distsqr = xd*xd + yd*yd + zd*zd;
            for (int i = 0; i < MaxChunkRenders; i++)
            {
                if (distsqr < chunkBuildRenderList[i][0] || p <= i)
                {
                    for (int j = MaxChunkRenders - 1; j >= i + 1; j--)
                    {
                        chunkBuildRenderList[j][0] = chunkBuildRenderList[j - 1][0];
                        chunkBuildRenderList[j][1] = chunkBuildRenderList[j - 1][1];
                    }
                    chunkBuildRenderList[i][0] = distsqr;
                    chunkBuildRenderList[i][1] = ci;
                    break;
                }
            }
            if (p < MaxChunkRenders) p++;
        }
    }
    chunkBuildRenders = p;
}

void sortChunkLoadUnloadList(int xpos, int ypos, int zpos)
{

    int cxp, cyp, czp, cx, cy, cz, pl = 0, pu = 0, i;
    int xd, yd, zd, distsqr, first, middle, last;

    cxp = getchunkpos(xpos);
    cyp = getchunkpos(ypos);
    czp = getchunkpos(zpos);

    for (size_t ci = 0; ci < chunks.size(); ci++)
    {
        cx = chunks[ci]->cx;
        cy = chunks[ci]->cy;
        cz = chunks[ci]->cz;
        if (!chunkInRange(cx, cy, cz, cxp, cyp, czp, ViewDistance + 1))
        {
            xd = cx * 16 + 7 - xpos;
            yd = cy * 16 + 7 - ypos;
            zd = cz * 16 + 7 - zpos;
            distsqr = xd*xd + yd*yd + zd*zd;

            first = 0;
            last = pl - 1;
            while (first <= last)
            {
                middle = (first + last) / 2;
                if (distsqr > chunkUnloadList[middle].second)last = middle - 1;
                else first = middle + 1;
            }
            if (first > pl || first >= MaxChunkUnloads) continue;
            i = first;

            for (int j = MaxChunkUnloads - 1; j > i; j--)
            {
                chunkUnloadList[j].first = chunkUnloadList[j - 1].first;
                chunkUnloadList[j].second = chunkUnloadList[j - 1].second;
            }
            chunkUnloadList[i].first = chunks[ci];
            chunkUnloadList[i].second = distsqr;

            if (pl < MaxChunkUnloads) pl++;
        }
    }
    chunkUnloads = pl;

    for (cx = cxp - ViewDistance - 1; cx <= cxp + ViewDistance; cx++)
    {
        for (cy = cyp - ViewDistance - 1; cy <= cyp + ViewDistance; cy++)
        {
            for (cz = czp - ViewDistance - 1; cz <= czp + ViewDistance; cz++)
            {
                if (chunkOutOfBound(cx, cy, cz)) continue;
                if (cpArray.getChunkPtr(cx, cy, cz) == nullptr)
                {
                    xd = cx * 16 + 7 - xpos;
                    yd = cy * 16 + 7 - ypos;
                    zd = cz * 16 + 7 - zpos;
                    distsqr = xd*xd + yd*yd + zd*zd;

                    first = 0;
                    last = pu - 1;
                    while (first <= last)
                    {
                        middle = (first + last) / 2;
                        if (distsqr < chunkLoadList[middle][0]) last = middle - 1;
                        else first = middle + 1;
                    }
                    if (first > pu || first >= MaxChunkLoads) continue;
                    i = first;

                    for (int j = MaxChunkLoads - 1; j > i; j--)
                    {
                        chunkLoadList[j][0] = chunkLoadList[j - 1][0];
                        chunkLoadList[j][1] = chunkLoadList[j - 1][1];
                        chunkLoadList[j][2] = chunkLoadList[j - 1][2];
                        chunkLoadList[j][3] = chunkLoadList[j - 1][3];
                    }
                    chunkLoadList[i][0] = distsqr;
                    chunkLoadList[i][1] = cx;
                    chunkLoadList[i][2] = cy;
                    chunkLoadList[i][3] = cz;
                    if (pu < MaxChunkLoads) pu++;
                }
            }
        }
    }
    chunkLoads = pu;
}

void calcVisible(double xpos, double ypos, double zpos, Frustum& frus)
{
    chunk::setRelativeBase(xpos, ypos, zpos, frus);
    for (size_t ci = 0; ci < chunks.size(); ci++) chunks[ci]->calcVisible();
}

void saveAllChunks()
{
    for (size_t i = 0; i < chunks.size(); i++)
    {
        chunks[i]->SaveToFile();
    }
}

void destroyAllChunks()
{

    for (size_t i = 0; i < chunks.size(); i++)
    {
        if (!chunks[i]->Empty)
        {
            chunks[i]->destroyRender();
            chunks[i]->destroy();
            delete chunks[i];
        }
    }
	chunks.clear();
	cpArray = chunkPtrArray();
    HMap.destroy();

    rebuiltChunks = 0;
    rebuiltChunksCount = 0;

    updatedChunks = 0;
    updatedChunksCount = 0;

    unloadedChunks = 0;
    unloadedChunksCount = 0;

    memset(chunkBuildRenderList, 0, 256 * 2 * sizeof(int));
    memset(chunkLoadList, 0, 256 * 4 * sizeof(int));

    chunkBuildRenders = 0;
    chunkLoads = 0;
    chunkUnloads = 0;

}

void buildtree(int x, int y, int z)
{
    //对生成条件进行更严格的检测
    //一：正上方五格必须为空气
    for (int i = y + 1; i < y + 6; i++)
    {
        if (getblock(x, i, z) != block(Blocks::AIR))return;
    }
    //二：周围五格不能有树
    for (int ix = x - 4; ix < x + 4; ix++)
    {
        for (int iy = y - 4; iy < y + 4; iy++)
        {
            for (int iz = z - 4; iz < z + 4; iz++)
            {
                if (getblock(ix, iy, iz) == block(Blocks::WOOD) || getblock(ix, iy, iz) == block(Blocks::LEAF))return;
            }
        }
    }
    //开始生成
    //设置泥土
    setblock(x, y, z, block(Blocks::DIRT));
    //设置树干
    int h = 0;//高度
    //测算泥土数量
    int Dirt = 0;//泥土数
    for (int ix = x - 4; ix < x + 4; ix++)
    {
        for (int iy = y - 4; iy < y; iy++)
        {
            for (int iz = z - 4; iz < z + 4; iz++)
            {
                if (getblock(ix, iy, iz) == block(Blocks::DIRT))Dirt++;
            }
        }
    }
    //测算最高高度
    for (int i = y + 1; i < y + 16; i++)
    {
        if (getblock(x, i, z) == block(Blocks::AIR))
        {
            h++;
        }
        else
        {
            break;
        }
    }
    //取最小值
    h = min(h, Dirt * 15 / 268 * max(rnd(), 0.8));
    if (h < 7)return;
    //开始生成树干
    for (int i = y + 1; i < y + h + 1; i++)
    {
        setblock(x, i, z, block(Blocks::WOOD));
    }
    //设置树叶及枝杈
    //计算树叶起始生成高度
    int leafh = int(double(h)*0.618) + 1;//黄金分割比大法好！！！
    int distancen2 = int(double((h - leafh + 1)*(h - leafh + 1))) + 1;
    for (int iy = y + leafh; iy < y + int(double(h)*1.382) + 2; iy++)
    {
        for (int ix = x - 6; ix < x + 6; ix++)
        {
            for (int iz = z - 6; iz < z + 6; iz++)
            {
                int distancen = Distancen(ix, iy, iz, x, y + leafh + 1, z);
                if ((getblock(ix, iy, iz) == block(Blocks::AIR)) && (distancen <distancen2))
                {
                    if ((distancen <= distancen2 / 9) && (rnd()>0.3))//生成枝杈
                    {
                        setblock(ix, iy, iz, block(Blocks::WOOD));
                    }
                    else//生成树叶
                    {
                        //鉴于残缺树叶的bug,不考虑树叶密度
                        setblock(ix, iy, iz, block(Blocks::LEAF));
                    }
                }
            }
        }
    }
}

void explode(int x, int y, int z, int r, chunk* c)
{
    double maxdistsqr = r*r;
    for (int fx = x - r - 1; fx < x + r + 1; fx++)
    {
        for (int fy = y - r - 1; fy < y + r + 1; fy++)
        {
            for (int fz = z - r - 1; fz < z + r + 1; fz++)
            {
                int distsqr = (fx - x)*(fx - x) + (fy - y)*(fy - y) + (fz - z)*(fz - z);
                if (distsqr <= maxdistsqr*0.75 ||
                        distsqr <= maxdistsqr && rnd() > (distsqr - maxdistsqr*0.6) / (maxdistsqr*0.4))
                {
                    block e = World::getblock(fx, fy, fz);
                    if (e == block(Blocks::AIR)) continue;
                    for (int j = 1; j <= 12; j++)
                    {
                        Particles::throwParticle(e,
                                                 float(fx + rnd() - 0.5f), float(fy + rnd() - 0.2f), float(fz + rnd() - 0.5f),
                                                 float(rnd()*0.2f - 0.1f), float(rnd()*0.2f - 0.1f), float(rnd()*0.2f - 0.1f),
                                                 float(rnd()*0.02 + 0.03), int(rnd() * 60) + 30);
                    }
                    Modifyblock(fx, fy, fz, block(Blocks::AIR), c);
                }
            }
        }
    }
}

vector<Blocks::BUDDP> blockupdatequery;

block* getblockptr(int x, int y, int z, block* mask)
{
    //获取方块
    int	cx = getchunkpos(x), cy = getchunkpos(y), cz = getchunkpos(z);
    if (chunkOutOfBound(cx, cy, cz)) return mask;
    int bx = getblockpos(x), by = getblockpos(y), bz = getblockpos(z);
    chunk* ci = getChunkPtr(cx, cy, cz);
    if (ci == EmptyChunkPtr) return mask;
    if (ci != nullptr ) return ci->pblocks + (bx * 256 + by * 16 + bz);
    return mask;
}

void MarkBlockUpdate(Blocks::BUDDP Block)
{
    blockupdatequery.push_back(Block);
}

void ExecBUPD(Blocks::BUDDP B)
{
    if (BlockInfo((*(B.slf))).ExecBUF(B))
	{
        getChunkPtr(getchunkpos(B.cx), getchunkpos(B.cy), getchunkpos(B.cz))->Modified = true;
        updateblock(B.cx, B.cy, B.cz, true);
        MarkBlockUpdate(Blocks::BUDDP(B.origon, B.slf, nullptr, B.dslf, nullptr, B.cx, B.cy, B.cz));
    }
}

void ProcessBuq()
{
    vector<Blocks::BUDDP> swap;
    swap.swap(blockupdatequery);
    blockupdatequery.clear();
    block Mask = block(Blocks::AIR);
    block* b;
    long long bx, by , bz;
    const int vec[6][3] = { { -1, 0, 0 },{ 1, 0, 0 },{ 0, -1, 0 },{ 0, 1, 0 },{ 0, 0, -1 },{ 0, 0, 1 } };
    
    for (Blocks::BUDDP B : swap)
    {
        bx = B.cx;
        by = B.cy;
        bz = B.cz;
        for (int i = 0; i < 6; i++)
        {
            b = getblockptr(bx + vec[i][0], by + vec[i][1], bz + vec[i][2], &Mask);
            if (b->ID != Blocks::AIR)
                ExecBUPD(Blocks::BUDDP(B.origon, B.upd, b, B.dudp, nullptr, bx + vec[i][0], by + vec[i][1], bz + vec[i][2] ));
        }
    }
}

void putblock(int x, int y, int z, block Block)
{
    setblock(x, y, z, Block);
}

void pickleaf()
{
    if (rnd() < 0.05)
    {
        if (rnd() < 0.5)
            Player::AddItem(APPLE);
        else
            Player::AddItem(STICK);
    }
    else
    {
        Player::AddItem(block(Blocks::LEAF));
    }
}

void picktree(int x, int y, int z)
{
    if (getblock(x, y, z) == block(Blocks::LEAF))
        pickleaf();
    else
        Player::AddItem(getblock(x, y, z));

    for (int j = 1; j <= 10; j++)
    {
        Particles::throwParticle(getblock(x, y, z),
                                 float(x + rnd() - 0.5f), float(y + rnd() - 0.2f), float(z + rnd() - 0.5f),
                                 float(rnd()*0.2f - 0.1f), float(rnd()*0.2f - 0.1f), float(rnd()*0.2f - 0.1f),
                                 float(rnd()*0.02 + 0.03), int(rnd() * 60) + 30);
    }
    Modifyblock(x, y, z, block(Blocks::AIR));
	int vec[5][3] = { {0, 1, 0}, {0, 0, 1}, {0, 0, -1}, {1, 0, 0}, {-1, 0, 0} };
	for (int i = 0; i < 5; i++)
		if (getblock(x + vec[i][0], y + vec[i][1], z + vec[i][2]) == block(Blocks::WOOD) || getblock(x + vec[i][0], y + vec[i][1], z + vec[i][2]) == block(Blocks::LEAF))
			picktree(x + vec[i][0], y + vec[i][1], z + vec[i][2]);
}

void pickblock(int x, int y, int z)
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
    if (getblock(x, y, z) == block(Blocks::LEAF))
        pickleaf();
    else
        Player::AddItem(getblock(x, y, z));

    Modifyblock(x, y, z, block(Blocks::AIR));
}

bool chunkInRange(int x, int y, int z, int px, int py, int pz, int dist)
{
    //检测给出的chunk坐标是否在渲染范围内
    return !(x<px - dist || x>px + dist - 1 || y<py - dist || y>py + dist - 1 || z<pz - dist || z>pz + dist - 1);
}

chunkid getChunkID(int x, int y, int z)
{
    if (y == -128) y = 0;
    if (y <= 0) y = abs(y) + (1LL << 7);
    if (x == -134217728) x = 0;
    if (x <= 0) x = abs(x) + (1LL << 27);
    if (z == -134217728) z = 0;
    if (z <= 0) z = abs(z) + (1LL << 27);
    return (chunkid(y) << 56) + (chunkid(x) << 28) + z;
}

bool chunkOutOfBound(int x, int y, int z)
{
    return y < -World::worldheight || y > World::worldheight - 1 ||
           x < -134217728 || x > 134217727 || z < -134217728 || z > 134217727;
}

bool chunkLoaded(int x, int y, int z)
{
    if (chunkOutOfBound(x, y, z))
        return false;
    if (getChunkPtr(x, y, z) != nullptr)
        return true;
    return false;
}

}

