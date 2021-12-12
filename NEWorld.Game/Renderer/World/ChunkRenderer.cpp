#include "ChunkRenderer.h"
#include "Renderer.h"
#include "Universe/World/World.h"

namespace ChunkRenderer {
    using World::getbrightness;

    void renderblock(int x, int y, int z, World::Chunk *chunkptr) {
        double colors, color1, color2, color3, color4, tcx, tcy, size, EPS = 0.0;
        const auto [gx, gy, gz] = (chunkptr->GetPosition() * 16 + Int3(x, y, z)).Data;
        Block blk[7] = {(chunkptr->GetBlock({x, y, z})),
                        z < 15 ? chunkptr->GetBlock({(x), (y), (z + 1)}) : World::GetBlock({(gx), (gy), (gz + 1)}, Blocks::ROCK),
                        z > 0 ? chunkptr->GetBlock({(x), (y), (z - 1)}) : World::GetBlock({(gx), (gy), (gz - 1)}, Blocks::ROCK),
                        x < 15 ? chunkptr->GetBlock({(x + 1), (y), (z)}) : World::GetBlock({(gx + 1), (gy), (gz)}, Blocks::ROCK),
                        x > 0 ? chunkptr->GetBlock({(x - 1), (y), (z)}) : World::GetBlock({(gx - 1), (gy), (gz)}, Blocks::ROCK),
                        y < 15 ? chunkptr->GetBlock({(x), (y + 1), (z)}) : World::GetBlock({(gx), (gy + 1), (gz)}, Blocks::ROCK),
                        y > 0 ? chunkptr->GetBlock({(x), (y - 1), (z)}) : World::GetBlock({(gx), (gy - 1), (gz)}, Blocks::ROCK)};

        Brightness brt[7] = {(chunkptr->GetBrightness({(x), (y), (z)})),
                             z < 15 ? chunkptr->GetBrightness({(x), (y), (z + 1)}) : World::getbrightness(gx, gy, gz + 1),
                             z > 0 ? chunkptr->GetBrightness({(x), (y), (z - 1)}) : World::getbrightness(gx, gy, gz - 1),
                             x < 15 ? chunkptr->GetBrightness({(x + 1), (y), (z)}) : World::getbrightness(gx + 1, gy, gz),
                             x > 0 ? chunkptr->GetBrightness({(x - 1), (y), (z)}) : World::getbrightness(gx - 1, gy, gz),
                             y < 15 ? chunkptr->GetBrightness({(x), (y + 1), (z)}) : World::getbrightness(gx, gy + 1, gz),
                             y > 0 ? chunkptr->GetBrightness({(x), (y - 1), (z)}) : World::getbrightness(gx, gy - 1, gz)};

        size = 1 / 8.0f - EPS;

        if (NiceGrass && blk[0] == Blocks::GRASS &&
            World::GetBlock({(gx), (gy - 1), (gz + 1)}, Blocks::ROCK, chunkptr) == Blocks::GRASS) {
            tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
        } else {
            tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
        }

        // Front Face
        if (!(BlockInfo(blk[1]).isOpaque() || (blk[1] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            colors = brt[1];
            color1 = colors;
            color2 = colors;
            color3 = colors;
            color4 = colors;

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting) {
                color1 = (colors + World::getbrightness(gx, gy - 1, gz + 1) + World::getbrightness(gx - 1, gy, gz + 1) +
                          World::getbrightness(gx - 1, gy - 1, gz + 1)) / 4.0;
                color2 = (colors + World::getbrightness(gx, gy - 1, gz + 1) + World::getbrightness(gx + 1, gy, gz + 1) +
                          World::getbrightness(gx + 1, gy - 1, gz + 1)) / 4.0;
                color3 = (colors + World::getbrightness(gx, gy + 1, gz + 1) + World::getbrightness(gx + 1, gy, gz + 1) +
                          World::getbrightness(gx + 1, gy + 1, gz + 1)) / 4.0;
                color4 = (colors + World::getbrightness(gx, gy + 1, gz + 1) + World::getbrightness(gx - 1, gy, gz + 1) +
                          World::getbrightness(gx - 1, gy + 1, gz + 1)) / 4.0;
            }

            color1 /= World::BRIGHTNESSMAX;
            color2 /= World::BRIGHTNESSMAX;
            color3 /= World::BRIGHTNESSMAX;
            color4 /= World::BRIGHTNESSMAX;
            if (blk[0] != Blocks::GLOWSTONE && !Renderer::AdvancedRender) {
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

        if (NiceGrass && blk[0] == Blocks::GRASS &&
            World::GetBlock({(gx), (gy - 1), (gz - 1)}, Blocks::ROCK, chunkptr) == Blocks::GRASS) {
            tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
        } else {
            tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
        }

        // Back Face
        if (!(BlockInfo(blk[2]).isOpaque() || (blk[2] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            colors = brt[2];
            color1 = colors;
            color2 = colors;
            color3 = colors;
            color4 = colors;

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting) {
                color1 = (colors + World::getbrightness(gx, gy - 1, gz - 1) + World::getbrightness(gx - 1, gy, gz - 1) +
                          World::getbrightness(gx - 1, gy - 1, gz - 1)) / 4.0;
                color2 = (colors + World::getbrightness(gx, gy + 1, gz - 1) + World::getbrightness(gx - 1, gy, gz - 1) +
                          World::getbrightness(gx - 1, gy + 1, gz - 1)) / 4.0;
                color3 = (colors + World::getbrightness(gx, gy + 1, gz - 1) + World::getbrightness(gx + 1, gy, gz - 1) +
                          World::getbrightness(gx + 1, gy + 1, gz - 1)) / 4.0;
                color4 = (colors + World::getbrightness(gx, gy - 1, gz - 1) + World::getbrightness(gx + 1, gy, gz - 1) +
                          World::getbrightness(gx + 1, gy - 1, gz - 1)) / 4.0;
            }

            color1 /= World::BRIGHTNESSMAX;
            color2 /= World::BRIGHTNESSMAX;
            color3 /= World::BRIGHTNESSMAX;
            color4 /= World::BRIGHTNESSMAX;
            if (blk[0] != Blocks::GLOWSTONE && !Renderer::AdvancedRender) {
                color1 *= 0.5;
                color2 *= 0.5;
                color3 *= 0.5;
                color4 *= 0.5;
            }

            if (Renderer::AdvancedRender) Renderer::Attrib1f(1.0f);
            Renderer::Color3d(color1, color1, color1);
            Renderer::TexCoord2d(tcx + size * 1.0, tcy + size * 0.0);
            Renderer::Vertex3d(-0.5 + x, -0.5 + y, -0.5 + z);
            Renderer::Color3d(color2, color2, color2);
            Renderer::TexCoord2d(tcx + size * 1.0, tcy + size * 1.0);
            Renderer::Vertex3d(-0.5 + x, 0.5 + y, -0.5 + z);
            Renderer::Color3d(color3, color3, color3);
            Renderer::TexCoord2d(tcx + size * 0.0, tcy + size * 1.0);
            Renderer::Vertex3d(0.5 + x, 0.5 + y, -0.5 + z);
            Renderer::Color3d(color4, color4, color4);
            Renderer::TexCoord2d(tcx + size * 0.0, tcy + size * 0.0);
            Renderer::Vertex3d(0.5 + x, -0.5 + y, -0.5 + z);

        }

        if (NiceGrass && blk[0] == Blocks::GRASS &&
            World::GetBlock({(gx + 1), (gy - 1), (gz)}, Blocks::ROCK, chunkptr) == Blocks::GRASS) {
            tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
        } else {
            tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
        }

        // Right face
        if (!(BlockInfo(blk[3]).isOpaque() || (blk[3] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            colors = brt[3];
            color1 = colors;
            color2 = colors;
            color3 = colors;
            color4 = colors;

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting) {
                color1 = (colors + World::getbrightness(gx + 1, gy - 1, gz) + World::getbrightness(gx + 1, gy, gz - 1) +
                          World::getbrightness(gx + 1, gy - 1, gz - 1)) / 4.0;
                color2 = (colors + World::getbrightness(gx + 1, gy + 1, gz) + World::getbrightness(gx + 1, gy, gz - 1) +
                          World::getbrightness(gx + 1, gy + 1, gz - 1)) / 4.0;
                color3 = (colors + World::getbrightness(gx + 1, gy + 1, gz) + World::getbrightness(gx + 1, gy, gz + 1) +
                          World::getbrightness(gx + 1, gy + 1, gz + 1)) / 4.0;
                color4 = (colors + World::getbrightness(gx + 1, gy - 1, gz) + World::getbrightness(gx + 1, gy, gz + 1) +
                          World::getbrightness(gx + 1, gy - 1, gz + 1)) / 4.0;
            }

            color1 /= World::BRIGHTNESSMAX;
            color2 /= World::BRIGHTNESSMAX;
            color3 /= World::BRIGHTNESSMAX;
            color4 /= World::BRIGHTNESSMAX;
            if (blk[0] != Blocks::GLOWSTONE && !Renderer::AdvancedRender) {
                color1 *= 0.7;
                color2 *= 0.7;
                color3 *= 0.7;
                color4 *= 0.7;
            }

            if (Renderer::AdvancedRender) Renderer::Attrib1f(2.0f);
            Renderer::Color3d(color1, color1, color1);
            Renderer::TexCoord2d(tcx + size * 1.0, tcy + size * 0.0);
            Renderer::Vertex3d(0.5 + x, -0.5 + y, -0.5 + z);
            Renderer::Color3d(color2, color2, color2);
            Renderer::TexCoord2d(tcx + size * 1.0, tcy + size * 1.0);
            Renderer::Vertex3d(0.5 + x, 0.5 + y, -0.5 + z);
            Renderer::Color3d(color3, color3, color3);
            Renderer::TexCoord2d(tcx + size * 0.0, tcy + size * 1.0);
            Renderer::Vertex3d(0.5 + x, 0.5 + y, 0.5 + z);
            Renderer::Color3d(color4, color4, color4);
            Renderer::TexCoord2d(tcx + size * 0.0, tcy + size * 0.0);
            Renderer::Vertex3d(0.5 + x, -0.5 + y, 0.5 + z);

        }

        if (NiceGrass && blk[0] == Blocks::GRASS &&
            World::GetBlock({(gx - 1), (gy - 1), (gz)}, Blocks::ROCK, chunkptr) == Blocks::GRASS) {
            tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
        } else {
            tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
        }

        // Left Face
        if (!(BlockInfo(blk[4]).isOpaque() || (blk[4] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            colors = brt[4];
            color1 = colors;
            color2 = colors;
            color3 = colors;
            color4 = colors;

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting) {
                color1 = (colors + World::getbrightness(gx - 1, gy - 1, gz) + World::getbrightness(gx - 1, gy, gz - 1) +
                          World::getbrightness(gx - 1, gy - 1, gz - 1)) / 4.0;
                color2 = (colors + World::getbrightness(gx - 1, gy - 1, gz) + World::getbrightness(gx - 1, gy, gz + 1) +
                          World::getbrightness(gx - 1, gy - 1, gz + 1)) / 4.0;
                color3 = (colors + World::getbrightness(gx - 1, gy + 1, gz) + World::getbrightness(gx - 1, gy, gz + 1) +
                          World::getbrightness(gx - 1, gy + 1, gz + 1)) / 4.0;
                color4 = (colors + World::getbrightness(gx - 1, gy + 1, gz) + World::getbrightness(gx - 1, gy, gz - 1) +
                          World::getbrightness(gx - 1, gy + 1, gz - 1)) / 4.0;
            }

            color1 /= World::BRIGHTNESSMAX;
            color2 /= World::BRIGHTNESSMAX;
            color3 /= World::BRIGHTNESSMAX;
            color4 /= World::BRIGHTNESSMAX;
            if (blk[0] != Blocks::GLOWSTONE && !Renderer::AdvancedRender) {
                color1 *= 0.7;
                color2 *= 0.7;
                color3 *= 0.7;
                color4 *= 0.7;
            }

            if (Renderer::AdvancedRender) Renderer::Attrib1f(3.0f);
            Renderer::Color3d(color1, color1, color1);
            Renderer::TexCoord2d(tcx + size * 0.0, tcy + size * 0.0);
            Renderer::Vertex3d(-0.5 + x, -0.5 + y, -0.5 + z);
            Renderer::Color3d(color2, color2, color2);
            Renderer::TexCoord2d(tcx + size * 1.0, tcy + size * 0.0);
            Renderer::Vertex3d(-0.5 + x, -0.5 + y, 0.5 + z);
            Renderer::Color3d(color3, color3, color3);
            Renderer::TexCoord2d(tcx + size * 1.0, tcy + size * 1.0);
            Renderer::Vertex3d(-0.5 + x, 0.5 + y, 0.5 + z);
            Renderer::Color3d(color4, color4, color4);
            Renderer::TexCoord2d(tcx + size * 0.0, tcy + size * 1.0);
            Renderer::Vertex3d(-0.5 + x, 0.5 + y, -0.5 + z);

        }

        tcx = Textures::getTexcoordX(blk[0], 1);
        tcy = Textures::getTexcoordY(blk[0], 1);

        // Top Face
        if (!(BlockInfo(blk[5]).isOpaque() || (blk[5] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            colors = brt[5];
            color1 = colors;
            color2 = colors;
            color3 = colors;
            color4 = colors;

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting) {
                color1 = (color1 + World::getbrightness(gx, gy + 1, gz - 1) + World::getbrightness(gx - 1, gy + 1, gz) +
                          World::getbrightness(gx - 1, gy + 1, gz - 1)) / 4.0;
                color2 = (color2 + World::getbrightness(gx, gy + 1, gz + 1) + World::getbrightness(gx - 1, gy + 1, gz) +
                          World::getbrightness(gx - 1, gy + 1, gz + 1)) / 4.0;
                color3 = (color3 + World::getbrightness(gx, gy + 1, gz + 1) + World::getbrightness(gx + 1, gy + 1, gz) +
                          World::getbrightness(gx + 1, gy + 1, gz + 1)) / 4.0;
                color4 = (color4 + World::getbrightness(gx, gy + 1, gz - 1) + World::getbrightness(gx + 1, gy + 1, gz) +
                          World::getbrightness(gx + 1, gy + 1, gz - 1)) / 4.0;
            }

            color1 /= World::BRIGHTNESSMAX;
            color2 /= World::BRIGHTNESSMAX;
            color3 /= World::BRIGHTNESSMAX;
            color4 /= World::BRIGHTNESSMAX;

            if (Renderer::AdvancedRender) Renderer::Attrib1f(4.0f);
            Renderer::Color3d(color1, color1, color1);
            Renderer::TexCoord2d(tcx + size * 0.0, tcy + size * 1.0);
            Renderer::Vertex3d(-0.5 + x, 0.5 + y, -0.5 + z);
            Renderer::Color3d(color2, color2, color2);
            Renderer::TexCoord2d(tcx + size * 0.0, tcy + size * 0.0);
            Renderer::Vertex3d(-0.5 + x, 0.5 + y, 0.5 + z);
            Renderer::Color3d(color3, color3, color3);
            Renderer::TexCoord2d(tcx + size * 1.0, tcy + size * 0.0);
            Renderer::Vertex3d(0.5 + x, 0.5 + y, 0.5 + z);
            Renderer::Color3d(color4, color4, color4);
            Renderer::TexCoord2d(tcx + size * 1.0, tcy + size * 1.0);
            Renderer::Vertex3d(0.5 + x, 0.5 + y, -0.5 + z);

        }

        tcx = Textures::getTexcoordX(blk[0], 3);
        tcy = Textures::getTexcoordY(blk[0], 3);

        // Bottom Face
        if (!(BlockInfo(blk[6]).isOpaque() || (blk[6] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            colors = brt[6];
            color1 = colors;
            color2 = colors;
            color3 = colors;
            color4 = colors;

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting) {
                color1 = (colors + World::getbrightness(gx, gy - 1, gz - 1) + World::getbrightness(gx - 1, gy - 1, gz) +
                          World::getbrightness(gx - 1, gy - 1, gz - 1)) / 4.0;
                color2 = (colors + World::getbrightness(gx, gy - 1, gz - 1) + World::getbrightness(gx + 1, gy - 1, gz) +
                          World::getbrightness(gx + 1, gy - 1, gz - 1)) / 4.0;
                color3 = (colors + World::getbrightness(gx, gy - 1, gz + 1) + World::getbrightness(gx + 1, gy - 1, gz) +
                          World::getbrightness(gx + 1, gy - 1, gz + 1)) / 4.0;
                color4 = (colors + World::getbrightness(gx, gy - 1, gz + 1) + World::getbrightness(gx - 1, gy - 1, gz) +
                          World::getbrightness(gx - 1, gy - 1, gz + 1)) / 4.0;
            }

            color1 /= World::BRIGHTNESSMAX;
            color2 /= World::BRIGHTNESSMAX;
            color3 /= World::BRIGHTNESSMAX;
            color4 /= World::BRIGHTNESSMAX;

            if (Renderer::AdvancedRender) Renderer::Attrib1f(5.0f);
            Renderer::Color3d(color1, color1, color1);
            Renderer::TexCoord2d(tcx + size * 1.0, tcy + size * 1.0);
            Renderer::Vertex3d(-0.5 + x, -0.5 + y, -0.5 + z);
            Renderer::Color3d(color2, color2, color2);
            Renderer::TexCoord2d(tcx + size * 0.0, tcy + size * 1.0);
            Renderer::Vertex3d(0.5 + x, -0.5 + y, -0.5 + z);
            Renderer::Color3d(color3, color3, color3);
            Renderer::TexCoord2d(tcx + size * 0.0, tcy + size * 0.0);
            Renderer::Vertex3d(0.5 + x, -0.5 + y, 0.5 + z);
            Renderer::Color3d(color4, color4, color4);
            Renderer::TexCoord2d(tcx + size * 1.0, tcy + size * 0.0);
            Renderer::Vertex3d(-0.5 + x, -0.5 + y, 0.5 + z);

        }
    }

    void RenderPrimitive_Depth(QuadPrimitive_Depth &p) {
        const auto x = p.x, y = p.y, z = p.z, length = p.length;
        if (p.direction == 0) {
            Renderer::Vertex3d(x + 0.5, y - 0.5, z - 0.5);
            Renderer::Vertex3d(x + 0.5, y + 0.5, z - 0.5);
            Renderer::Vertex3d(x + 0.5, y + 0.5, z + length + 0.5);
            Renderer::Vertex3d(x + 0.5, y - 0.5, z + length + 0.5);
        } else if (p.direction == 1) {
            Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
            Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
            Renderer::Vertex3d(x - 0.5, y - 0.5, z + length + 0.5);
            Renderer::Vertex3d(x - 0.5, y + 0.5, z + length + 0.5);
        } else if (p.direction == 2) {
            Renderer::Vertex3d(x + 0.5, y + 0.5, z - 0.5);
            Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
            Renderer::Vertex3d(x - 0.5, y + 0.5, z + length + 0.5);
            Renderer::Vertex3d(x + 0.5, y + 0.5, z + length + 0.5);
        } else if (p.direction == 3) {
            Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
            Renderer::Vertex3d(x + 0.5, y - 0.5, z - 0.5);
            Renderer::Vertex3d(x + 0.5, y - 0.5, z + length + 0.5);
            Renderer::Vertex3d(x - 0.5, y - 0.5, z + length + 0.5);
        } else if (p.direction == 4) {
            Renderer::Vertex3d(x - 0.5, y + 0.5, z + 0.5);
            Renderer::Vertex3d(x - 0.5, y - 0.5, z + 0.5);
            Renderer::Vertex3d(x + length + 0.5, y - 0.5, z + 0.5);
            Renderer::Vertex3d(x + length + 0.5, y + 0.5, z + 0.5);
        } else if (p.direction == 5) {
            Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
            Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
            Renderer::Vertex3d(x + length + 0.5, y + 0.5, z - 0.5);
            Renderer::Vertex3d(x + length + 0.5, y - 0.5, z - 0.5);
        }
    }

    void RenderChunk(World::Chunk *c) {
        int x, y, z;
        if (Renderer::AdvancedRender) Renderer::Init(2, 3, 1); else Renderer::Init(2, 3);
        for (x = 0; x < 16; x++) {
            for (y = 0; y < 16; y++) {
                for (z = 0; z < 16; z++) {
                    const auto curr = c->GetBlock({x, y, z});
                    if (curr == Blocks::ENV) continue;
                    if (!BlockInfo(curr).isTranslucent()) renderblock(x, y, z, c);
                }
            }
        }
        Renderer::Flush(c->vbuffer[0], c->vertexes[0]);
        if (Renderer::AdvancedRender) Renderer::Init(2, 3, 1); else Renderer::Init(2, 3);
        for (x = 0; x < 16; x++) {
            for (y = 0; y < 16; y++) {
                for (z = 0; z < 16; z++) {
                    const auto curr = c->GetBlock({x, y, z});
                    if (curr == Blocks::ENV) continue;
                    if (BlockInfo(curr).isTranslucent() && BlockInfo(curr).isSolid()) renderblock(x, y, z, c);
                }
            }
        }
        Renderer::Flush(c->vbuffer[1], c->vertexes[1]);
        if (Renderer::AdvancedRender) Renderer::Init(2, 3, 1); else Renderer::Init(2, 3);
        for (x = 0; x < 16; x++) {
            for (y = 0; y < 16; y++) {
                for (z = 0; z < 16; z++) {
                    const auto curr = c->GetBlock({x, y, z});
                    if (curr == Blocks::ENV) continue;
                    if (!BlockInfo(curr).isSolid()) renderblock(x, y, z, c);
                }
            }
        }
        Renderer::Flush(c->vbuffer[2], c->vertexes[2]);
    }

    void RenderDepthModel(World::Chunk *c) {
        const auto cp = c->GetPosition();
        auto x = 0, y = 0, z = 0;
        QuadPrimitive_Depth cur;
        Block bl, neighbour;
        auto valid = false;
        int cur_l_mx = bl = neighbour = 0;
        //Linear merge for depth model
        Renderer::Init(0, 0);
        for (auto d = 0; d < 6; d++) {
            cur.direction = d;
            for (auto i = 0; i < 16; i++)
                for (auto j = 0; j < 16; j++) {
                    for (auto k = 0; k < 16; k++) {
                        //Get position
                        if (d < 2) x = i, y = j, z = k;
                        else if (d < 4) x = i, y = j, z = k;
                        else x = k, y = i, z = j;
                        //Get block ID
                        bl = c->GetBlock({x, y, z});
                        //Get neighbour ID
                        const auto xx = x + delta[d][0], yy = y + delta[d][1], zz = z + delta[d][2];
                        const auto [gx, gy, gz] = (cp * 16 + Int3(xx, yy, zz)).Data;
                        if (xx < 0 || xx >= 16 || yy < 0 || yy >= 16 || zz < 0 || zz >= 16) {
                            neighbour = World::GetBlock({(gx), (gy), (gz)});
                        }
                        else {
                            neighbour = c->GetBlock({(xx), (yy), (zz)});
                        }
                        //Render
                        if (bl == Blocks::ENV || bl == Blocks::GLASS || bl == neighbour && bl != Blocks::LEAF ||
                            BlockInfo(neighbour).isOpaque() || BlockInfo(bl).isTranslucent()) {
                            //Not valid block
                            if (valid) {
                                if (BlockInfo(neighbour).isOpaque()) {
                                    if (cur_l_mx < cur.length) cur_l_mx = cur.length;
                                    cur_l_mx++;
                                } else {
                                    RenderPrimitive_Depth(cur);
                                    valid = false;
                                }
                            }
                            continue;
                        }
                        if (valid) {
                            if (cur_l_mx > cur.length) cur.length = cur_l_mx;
                            cur.length++;
                        } else {
                            valid = true;
                            cur.x = x;
                            cur.y = y;
                            cur.z = z;
                            cur.length = cur_l_mx = 0;
                        }
                    }
                    if (valid) {
                        RenderPrimitive_Depth(cur);
                        valid = false;
                    }
                }
        }
        Renderer::Flush(c->vbuffer[3], c->vertexes[3]);
    }
}