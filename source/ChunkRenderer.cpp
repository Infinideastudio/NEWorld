#include "Textures.h"
#include "Renderer.h"
#include "World.h"
#include "Chunk.h"

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
/*
合并面的顶点顺序（以0到3标出）：

The vertex order of merge face render
Numbered from 0 to 3:

(k++)
...
|    |
+----+--
|    |
|    |    |
3----2----+-
|curr|    |   ...
|face|    |   (j++)
0----1----+--

--qiaozhanrong
*/

void RenderPrimitive(QuadPrimitive& p)
{
    float col0 = (float)p.col0 * 0.25f / World::BRIGHTNESSMAX;
    float col1 = (float)p.col1 * 0.25f / World::BRIGHTNESSMAX;
    float col2 = (float)p.col2 * 0.25f / World::BRIGHTNESSMAX;
    float col3 = (float)p.col3 * 0.25f / World::BRIGHTNESSMAX;
    int x = p.x, y = p.y, z = p.z, length = p.length;
#ifdef NERDMODE1
    Renderer::TexCoord3d(0.0, 0.0, (p.tex + 0.5) / 64.0);
    if (p.direction == 0)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(2.0f);
        else col0 *= 0.7f, col1 *= 0.7f, col2 *= 0.7f, col3 *= 0.7f;
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z - 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z + length + 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z + length + 0.5);
    }
    else if (p.direction == 1)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(3.0f);
        else col0 *= 0.7f, col1 *= 0.7f, col2 *= 0.7f, col3 *= 0.7f;
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + length + 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + length + 0.5);
    }
    else if (p.direction == 2)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(4.0f);
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + length + 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z + length + 0.5);
    }
    else if (p.direction == 3)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(5.0f);
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z - 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z + length + 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + length + 0.5);
    }
    else if (p.direction == 4)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(0.0f);
        else col0 *= 0.5f, col1 *= 0.5f, col2 *= 0.5f, col3 *= 0.5f;
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x + length + 0.5, y - 0.5, z + 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x + length + 0.5, y + 0.5, z + 0.5);
    }
    else if (p.direction == 5)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(1.0f);
        else col0 *= 0.5f, col1 *= 0.5f, col2 *= 0.5f, col3 *= 0.5f;
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x + length + 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x + length + 0.5, y - 0.5, z - 0.5);
    }
#else
    float T3d = (Textures::getTextureIndex(p.block, face) - 0.5) / 64.0;
    switch (p.direction)
    {
    case 0:
    {
        if (p.block != block(Blocks::GLOWSTONE)) color *= 0.7;
        float geomentry[] =
        {
            0.0, 0.0, T3d, color, color, color, x + 0.5, y - 0.5, z - 0.5,
            0.0, 1.0, T3d, color, color, color, x + 0.5, y + 0.5, z - 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x + 0.5, y + 0.5, z + length + 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x + 0.5, y - 0.5, z + length + 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    case 1:
    {
        if (p.block != block(Blocks::GLOWSTONE)) color *= 0.7;
        float geomentry[] =
        {
            0.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z - 0.5,
            0.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z - 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z + length + 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z + length + 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    case 2:
    {
        float geomentry[] =
        {
            0.0, 0.0, T3d, color, color, color, x + 0.5, y + 0.5, z - 0.5,
            0.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z - 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z + length + 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x + 0.5, y + 0.5, z + length + 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    case 3:
    {
        float geomentry[] =
        {
            0.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z - 0.5,
            0.0, 1.0, T3d, color, color, color, x + 0.5, y - 0.5, z - 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x + 0.5, y - 0.5, z + length + 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z + length + 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    case 4:
    {
        if (p.block != block(Blocks::GLOWSTONE)) color *= 0.5;
        float geomentry[] =
        {
            0.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z + 0.5,
            0.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z + 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x + length + 0.5, y - 0.5, z + 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x + length + 0.5, y + 0.5, z + 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    case 5:
    {
        if (p.block != block(Blocks::GLOWSTONE)) color *= 0.5;
        float geomentry[] =
        {
            0.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z - 0.5,
            0.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z - 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x + length + 0.5, y + 0.5, z - 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x + length + 0.5, y - 0.5, z - 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    }
#endif // NERDMODE1

}

void RenderPrimitive_Depth(QuadPrimitive_Depth& p)
{
    int x = p.x, y = p.y, z = p.z, length = p.length;
    if (p.direction == 0)
    {
        Renderer::Vertex3d(x + 0.5, y - 0.5, z - 0.5);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z + length + 0.5);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z + length + 0.5);
    }
    else if (p.direction == 1)
    {
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + length + 0.5);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + length + 0.5);
    }
    else if (p.direction == 2)
    {
        Renderer::Vertex3d(x + 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + length + 0.5);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z + length + 0.5);
    }
    else if (p.direction == 3)
    {
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z - 0.5);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z + length + 0.5);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + length + 0.5);
    }
    else if (p.direction == 4)
    {
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + 0.5);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + 0.5);
        Renderer::Vertex3d(x + length + 0.5, y - 0.5, z + 0.5);
        Renderer::Vertex3d(x + length + 0.5, y + 0.5, z + 0.5);
    }
    else if (p.direction == 5)
    {
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x + length + 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x + length + 0.5, y - 0.5, z - 0.5);
    }
}

void World::chunk::Render()
{
    int x, y, z;
    if (Renderer::AdvancedRender) Renderer::Init(2, 3, 1);
    else Renderer::Init(2, 3);
    for (x = 0; x < 16; x++)
    {
        for (y = 0; y < 16; y++)
        {
            for (z = 0; z < 16; z++)
            {
                block curr = getblock(x, y, z);
                if (curr == block(Blocks::AIR)) continue;
                if (!BlockInfo(curr).isTranslucent()) renderblock(x, y, z, this);
            }
        }
    }
    Renderer::Flush(vbuffer[0], vertexes[0]);
    if (Renderer::AdvancedRender) Renderer::Init(2, 3, 1);
    else Renderer::Init(2, 3);
    for (x = 0; x < 16; x++)
    {
        for (y = 0; y < 16; y++)
        {
            for (z = 0; z < 16; z++)
            {
                block curr = getblock(x, y, z);
                if (curr == block(Blocks::AIR)) continue;
                if (BlockInfo(curr).isTranslucent() && BlockInfo(curr).isSolid()) renderblock(x, y, z, this);
            }
        }
    }
    Renderer::Flush(vbuffer[1], vertexes[1]);
    if (Renderer::AdvancedRender) Renderer::Init(2, 3, 1);
    else Renderer::Init(2, 3);
    for (x = 0; x < 16; x++)
    {
        for (y = 0; y < 16; y++)
        {
            for (z = 0; z < 16; z++)
            {
                block curr = getblock(x, y, z);
                if (curr == block(Blocks::AIR)) continue;
                if (!BlockInfo(curr).isSolid()) renderblock(x, y, z, this);
            }
        }
    }
    Renderer::Flush(vbuffer[2], vertexes[2]);
}

//合并面大法好！！！
void World::chunk::MergeFaceRender()
{
    //话说我注释一会中文一会英文是不是有点奇怪。。。
    // -- qiaozhanrong

    int gx = 0, gy = 0, gz = 0;
    int x = 0, y = 0, z = 0, cur_l_mx, br;
    int col0 = 0, col1 = 0, col2 = 0, col3 = 0;
    QuadPrimitive cur;
    block bl, neighbour;
    ubyte face = 0;
    TextureID tex;
    bool valid = false;
    for (int steps = 0; steps < 3; steps++)
    {
        cur = QuadPrimitive();
        cur_l_mx = 0;
        bl = neighbour = block(0);
        //Linear merge
        if (Renderer::AdvancedRender) Renderer::Init(3, 3, 1);
        else Renderer::Init(3, 3);
        for (int d = 0; d < 6; d++)
        {
            cur.direction = d;
            if (d == 2) face = 1;
            else if (d == 3) face = 3;
            else face = 2;
            //Render current face
            for (int i = 0; i < 16; i++)
                for (int j = 0; j < 16; j++)
                {
                    for (int k = 0; k < 16; k++)
                    {
                        //Get position & brightness
                        if (d == 0)   //x+
                        {
                            x = i, y = j, z = k;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx + 1, gy, gz, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx + 1, gy - 1, gz, this) + World::getbrightness(gx + 1, gy, gz - 1, this) + World::getbrightness(gx + 1, gy - 1, gz - 1, this);
                                col1 = br + World::getbrightness(gx + 1, gy + 1, gz, this) + World::getbrightness(gx + 1, gy, gz - 1, this) + World::getbrightness(gx + 1, gy + 1, gz - 1, this);
                                col2 = br + World::getbrightness(gx + 1, gy + 1, gz, this) + World::getbrightness(gx + 1, gy, gz + 1, this) + World::getbrightness(gx + 1, gy + 1, gz + 1, this);
                                col3 = br + World::getbrightness(gx + 1, gy - 1, gz, this) + World::getbrightness(gx + 1, gy, gz + 1, this) + World::getbrightness(gx + 1, gy - 1, gz + 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        else if (d == 1)   //x-
                        {
                            x = i, y = j, z = k;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx - 1, gy, gz, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx - 1, gy + 1, gz, this) + World::getbrightness(gx - 1, gy, gz - 1, this) + World::getbrightness(gx - 1, gy + 1, gz - 1, this);
                                col1 = br + World::getbrightness(gx - 1, gy - 1, gz, this) + World::getbrightness(gx - 1, gy, gz - 1, this) + World::getbrightness(gx - 1, gy - 1, gz - 1, this);
                                col2 = br + World::getbrightness(gx - 1, gy - 1, gz, this) + World::getbrightness(gx - 1, gy, gz + 1, this) + World::getbrightness(gx - 1, gy - 1, gz + 1, this);
                                col3 = br + World::getbrightness(gx - 1, gy + 1, gz, this) + World::getbrightness(gx - 1, gy, gz + 1, this) + World::getbrightness(gx - 1, gy + 1, gz + 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        else if (d == 2)   //y+
                        {
                            x = j, y = i, z = k;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx, gy + 1, gz, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx + 1, gy + 1, gz, this) + World::getbrightness(gx, gy + 1, gz - 1, this) + World::getbrightness(gx + 1, gy + 1, gz - 1, this);
                                col1 = br + World::getbrightness(gx - 1, gy + 1, gz, this) + World::getbrightness(gx, gy + 1, gz - 1, this) + World::getbrightness(gx - 1, gy + 1, gz - 1, this);
                                col2 = br + World::getbrightness(gx - 1, gy + 1, gz, this) + World::getbrightness(gx, gy + 1, gz + 1, this) + World::getbrightness(gx - 1, gy + 1, gz + 1, this);
                                col3 = br + World::getbrightness(gx + 1, gy + 1, gz, this) + World::getbrightness(gx, gy + 1, gz + 1, this) + World::getbrightness(gx + 1, gy + 1, gz + 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        else if (d == 3)   //y-
                        {
                            x = j, y = i, z = k;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx, gy - 1, gz, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx - 1, gy - 1, gz, this) + World::getbrightness(gx, gy - 1, gz - 1, this) + World::getbrightness(gx - 1, gy - 1, gz - 1, this);
                                col1 = br + World::getbrightness(gx + 1, gy - 1, gz, this) + World::getbrightness(gx, gy - 1, gz - 1, this) + World::getbrightness(gx + 1, gy - 1, gz - 1, this);
                                col2 = br + World::getbrightness(gx + 1, gy - 1, gz, this) + World::getbrightness(gx, gy - 1, gz + 1, this) + World::getbrightness(gx + 1, gy - 1, gz + 1, this);
                                col3 = br + World::getbrightness(gx - 1, gy - 1, gz, this) + World::getbrightness(gx, gy - 1, gz + 1, this) + World::getbrightness(gx - 1, gy - 1, gz + 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        else if (d == 4)   //z+
                        {
                            x = k, y = j, z = i;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx, gy, gz + 1, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx - 1, gy, gz + 1, this) + World::getbrightness(gx, gy + 1, gz + 1, this) + World::getbrightness(gx - 1, gy + 1, gz + 1, this);
                                col1 = br + World::getbrightness(gx - 1, gy, gz + 1, this) + World::getbrightness(gx, gy - 1, gz + 1, this) + World::getbrightness(gx - 1, gy - 1, gz + 1, this);
                                col2 = br + World::getbrightness(gx + 1, gy, gz + 1, this) + World::getbrightness(gx, gy - 1, gz + 1, this) + World::getbrightness(gx + 1, gy - 1, gz + 1, this);
                                col3 = br + World::getbrightness(gx + 1, gy, gz + 1, this) + World::getbrightness(gx, gy + 1, gz + 1, this) + World::getbrightness(gx + 1, gy + 1, gz + 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        else if (d == 5)   //z-
                        {
                            x = k, y = j, z = i;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx, gy, gz - 1, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx - 1, gy, gz - 1, this) + World::getbrightness(gx, gy - 1, gz - 1, this) + World::getbrightness(gx - 1, gy - 1, gz - 1, this);
                                col1 = br + World::getbrightness(gx - 1, gy, gz - 1, this) + World::getbrightness(gx, gy + 1, gz - 1, this) + World::getbrightness(gx - 1, gy + 1, gz - 1, this);
                                col2 = br + World::getbrightness(gx + 1, gy, gz - 1, this) + World::getbrightness(gx, gy + 1, gz - 1, this) + World::getbrightness(gx + 1, gy + 1, gz - 1, this);
                                col3 = br + World::getbrightness(gx + 1, gy, gz - 1, this) + World::getbrightness(gx, gy - 1, gz - 1, this) + World::getbrightness(gx + 1, gy - 1, gz - 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        //Get block ID
                        bl = getblock(x, y, z);
                        tex = Textures::getTextureIndex(bl, face);
                        neighbour = World::getblock(gx + delta[d][0], gy + delta[d][1], gz + delta[d][2], block(Blocks::ROCK), this);
                        if (NiceGrass && bl == block(Blocks::GRASS))
                        {
                            if (d == 0 && World::getblock(gx + 1, gy - 1, gz, block(Blocks::ROCK), this) == block(Blocks::GRASS)) tex = Textures::getTextureIndex(bl, 1);
                            else if (d == 1 && World::getblock(gx - 1, gy - 1, gz, block(Blocks::ROCK), this) == block(Blocks::GRASS)) tex = Textures::getTextureIndex(bl, 1);
                            else if (d == 4 && World::getblock(gx, gy - 1, gz + 1, block(Blocks::ROCK), this) == block(Blocks::GRASS)) tex = Textures::getTextureIndex(bl, 1);
                            else if (d == 5 && World::getblock(gx, gy - 1, gz - 1, block(Blocks::ROCK), this) == block(Blocks::GRASS)) tex = Textures::getTextureIndex(bl, 1);
                        }
                        //Render
                        const Blocks::SingleBlock& info = BlockInfo(bl);
                        if (bl == block(Blocks::AIR) || bl == neighbour && bl != block(Blocks::LEAF) || BlockInfo(neighbour).isOpaque() ||
                                steps == 0 && info.isTranslucent() ||
                                steps == 1 && (!info.isTranslucent() || !info.isSolid()) ||
                                steps == 2 && (!info.isTranslucent() || info.isSolid()))
                        {
                            //Not valid block
                            if (valid)
                            {
                                if (BlockInfo(neighbour).isOpaque() && !cur.once)
                                {
                                    if (cur_l_mx < cur.length) cur_l_mx = cur.length;
                                    cur_l_mx++;
                                }
                                else
                                {
                                    RenderPrimitive(cur);
                                    valid = false;
                                }
                            }
                            continue;
                        }
                        if (valid)
                        {
                            if (col0 != col1 || col1 != col2 || col2 != col3 || cur.once || tex != cur.tex || col0 != cur.col0)
                            {
                                RenderPrimitive(cur);
                                cur.x = x;
                                cur.y = y;
                                cur.z = z;
                                cur.length = cur_l_mx = 0;
                                cur.tex = tex;
                                cur.col0 = col0;
                                cur.col1 = col1;
                                cur.col2 = col2;
                                cur.col3 = col3;
                                if (col0 != col1 || col1 != col2 || col2 != col3) cur.once = true;
                                else cur.once = false;
                            }
                            else
                            {
                                if (cur_l_mx > cur.length) cur.length = cur_l_mx;
                                cur.length++;
                            }
                        }
                        else
                        {
                            valid = true;
                            cur.x = x;
                            cur.y = y;
                            cur.z = z;
                            cur.length = cur_l_mx = 0;
                            cur.tex = tex;
                            cur.col0 = col0;
                            cur.col1 = col1;
                            cur.col2 = col2;
                            cur.col3 = col3;
                            if (col0 != col1 || col1 != col2 || col2 != col3) cur.once = true;
                            else cur.once = false;
                        }
                    }
                    if (valid)
                    {
                        RenderPrimitive(cur);
                        valid = false;
                    }
                }
        }
        Renderer::Flush(vbuffer[steps], vertexes[steps]);
    }
}

void World::chunk::RenderDepthModel()
{
    int x = 0, y = 0, z = 0;
    QuadPrimitive_Depth cur;
    int cur_l_mx;
    block bl, neighbour;
    bool valid = false;
    cur_l_mx = 0;
    bl = neighbour = block(0);
    //Linear merge for depth model
    Renderer::Init(0, 0);
    for (int d = 0; d < 6; d++)
    {
        cur.direction = d;
        for (int i = 0; i < 16; i++)
            for (int j = 0; j < 16; j++)
            {
                for (int k = 0; k < 16; k++)
                {
                    //Get position
                    if (d < 2) x = i, y = j, z = k;
                    else if (d < 4) x = i, y = j, z = k;
                    else x = k, y = i, z = j;
                    //Get block ID
                    bl = getblock(x, y, z);
                    //Get neighbour ID
                    int xx = x + delta[d][0], yy = y + delta[d][1], zz = z + delta[d][2];
                    int gx = cx * 16 + xx, gy = cy * 16 + yy, gz = cz * 16 + zz;
                    if (xx < 0 || xx >= 16 || yy < 0 || yy >= 16 || zz < 0 || zz >= 16)
                        neighbour = World::getblock(gx, gy, gz);
                    else neighbour = getblock(xx, yy, zz);
                    //Render
                    if (bl == block(Blocks::AIR) || bl == block(Blocks::GLASS) || bl == neighbour && bl != block(Blocks::LEAF) ||
                            BlockInfo(neighbour).isOpaque() || BlockInfo(bl).isTranslucent())
                    {
                        //Not valid block
                        if (valid)
                        {
                            if (BlockInfo(neighbour).isOpaque())
                            {
                                if (cur_l_mx < cur.length) cur_l_mx = cur.length;
                                cur_l_mx++;
                            }
                            else
                            {
                                RenderPrimitive_Depth(cur);
                                valid = false;
                            }
                        }
                        continue;
                    }
                    if (valid)
                    {
                        if (cur_l_mx > cur.length) cur.length = cur_l_mx;
                        cur.length++;
                    }
                    else
                    {
                        valid = true;
                        cur.x = x;
                        cur.y = y;
                        cur.z = z;
                        cur.length = cur_l_mx = 0;
                    }
                }
                if (valid)
                {
                    RenderPrimitive_Depth(cur);
                    valid = false;
                }
            }
    }
    Renderer::Flush(vbuffer[3], vertexes[3]);
}