#ifndef CHUNKRENDERER_H
#define CHUNKRENDERER_H

#include "Definitions.h"
#include "Blocks.h"
#include "Textures.h"

namespace World
{
class chunk;
}

namespace ChunkRenderer
{
const int delta[6][3] = { { 1,0,0 },{ -1,0,0 },{ 0,1,0 },{ 0,-1,0 },{ 0,0,1 },{ 0,0,-1 } };

//合并面的一整个面 | One face in merge face
struct QuadPrimitive
{
    int x, y, z, length, direction;
    /*
    * 如果顶点颜色不同（平滑光照启用时），这个方形就不能和别的方形拼合起来。
    * 这个变量同时意味着四个顶点颜色是否不同。
    * If the vertexes have different colors (smoothed lighting), the primitive cannot connect with others.
    * This variable also means whether the vertexes have different colors.
    */
    bool once;
    //顶点颜色 | Vertex colors
    int col0, col1, col2, col3;
    //纹理ID | Texture ID
    TextureID tex;
    QuadPrimitive() : x(0), y(0), z(0), length(0), direction(0), once(false),
        tex(Textures::NULLBLOCK), col0(0), col1(0), col2(0), col3(0) {}
};

//深度模型的面 | Face in depth model
struct QuadPrimitive_Depth
{
    int x, y, z, length, direction;
    QuadPrimitive_Depth() : x(0), y(0), z(0), length(0), direction(0) {}
};

void RenderPrimitive(QuadPrimitive& p);
void RenderPrimitive_Depth(QuadPrimitive_Depth& p);
void RenderChunk(World::chunk* c);
void MergeFaceRender(World::chunk* c);
void RenderDepthModel(World::chunk* c);

}

#endif
