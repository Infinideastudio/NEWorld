/*
 * NEWorld: An free game with similar rules to Minecraft.
 * Copyright (C) 2016 NEWorld Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Textures.h"

int BLOCKTEXTURE_SIZE = 256, BLOCKTEXTURE_UNITSIZE = 32, BLOCKTEXTURE_UNITS = 8;

ubyte Textures::getTextureIndex(block blockname, ubyte side)
{
    switch (blockname.ID)
    {
    case Blocks::ROCK:
        return ROCK;
    case Blocks::GRASS:
        switch (side)
        {
        case 1:
            return GRASS_TOP;
        case 2:
            return GRASS_SIDE;
        case 3:
            return DIRT;
        }
    case Blocks::DIRT:
        return DIRT;
    case Blocks::STONE:
        return STONE;
    case Blocks::PLANK:
        return PLANK;
    case Blocks::WOOD:
        switch (side)
        {
        case 1:
            return WOOD_TOP;
        case 2:
            return WOOD_SIDE;
        case 3:
            return WOOD_TOP;
        }
    case Blocks::BEDROCK:
        return BEDROCK;
    case Blocks::LEAF:
        return LEAF;
    case Blocks::GLASS:
        return GLASS;
    case Blocks::WATER:
        return WATER;
    case Blocks::LAVA:
        return LAVA;
    case Blocks::GLOWSTONE:
        return GLOWSTONE;
    case Blocks::SAND:
        return SAND;
    case Blocks::CEMENT:
        return CEMENT;
    case Blocks::ICE:
        return ICE;
    case Blocks::COAL:
        return COAL;
    case Blocks::IRON:
        return IRON;
    case Blocks::TNT:
        return TNT;
    default:
        return NULLBLOCK;
    }
}

TextureID Textures::LoadBlock3DTexture(string Filename, string MkFilename)
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_3D);
    TextureID ret;
    glGenTextures(1, &ret);
    glBindTexture(GL_TEXTURE_3D, ret);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    TEXTURE_RGBA image(Filename, MkFilename);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, BLOCKTEXTURE_UNITSIZE, BLOCKTEXTURE_UNITSIZE, BLOCKTEXTURE_UNITS * BLOCKTEXTURE_UNITS, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.buffer.get());
    glDisable(GL_TEXTURE_3D);
    glEnable(GL_TEXTURE_2D);
    return ret;
}

void Textures::Build2DMipmaps(GLenum format, int w, int h, int level, const ubyte* src)
{
    int sum = 0, scale = 1, cur_w = 0, cur_h = 0, cc = 0;
    if (format == GL_RGBA) cc = 4;
    else if (format == GL_RGB) cc = 3;
    ubyte *cur = new ubyte[w*h*cc];
    memset(cur, 0, w*h*cc);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, level);
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, 0.0f);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, src);
    for (int i = 1; i <= level; i++)
    {
        scale <<= 1;
        cur_w = w / scale;
        cur_h = h / scale;
        for (int y = 0; y < cur_h; y++) for (int x = 0; x < cur_w; x++)
                for (int col = 0; col < cc; col++)
                {
                    sum = 0;
                    for (int yy = 0; yy < scale; yy++) for (int xx = 0; xx < scale; xx++)
                            sum += src[((y * scale + yy) * w + x * scale + xx) * cc + col];
                    cur[(y * cur_w + x) * cc + col] = (ubyte)(sum / (scale*scale));
                }
        glTexImage2D(GL_TEXTURE_2D, i, format, cur_w, cur_h, 0, format, GL_UNSIGNED_BYTE, cur);
    }
    delete[] cur;
}
