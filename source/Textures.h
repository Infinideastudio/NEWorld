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

#ifndef TEXTURES_H
#define TEXTURES_H

#include "Definitions.h"
#include "Blocks.h"
#include "Items.h"
#include "GLProc.h"

extern int BLOCKTEXTURE_SIZE, BLOCKTEXTURE_UNITSIZE;
const short BITMAP_ID = 0x4D42;

class Textures
{
public:
#pragma pack(push)
#pragma pack(1)
    struct BITMAPINFOHEADER
    {
        int biSize = 40, biWidth, biHeight;
        short biPlanes = 1, biBitCount = 24;
        int biCompression = 0, biSizeImage, biXPelsPerMeter = 0, biYPelsPerMeter = 0, biClrUsed = 0, biClrImportant = 0;
    };

    struct BITMAPFILEHEADER
    {
        short bfType = BITMAP_ID;
        int bfSize;
        short bfReserved1 = 0, bfReserved2 = 0;
        int bfOffBits = 54;
    };
#pragma pack(pop)

    struct TEXTURE_RGB
    {
        unsigned int sizeX;
        unsigned int sizeY;
        unique_ptr<ubyte[]> buffer;
        TEXTURE_RGB()
        {
        }
        TEXTURE_RGB(unsigned int _sizeX, unsigned int _sizeY)
            :sizeX(_sizeX), sizeY(_sizeY)
        {
            buffer = unique_ptr<ubyte[]>(new ubyte[_sizeX * _sizeY * 3]);
        }
        TEXTURE_RGB(const std::string& filename)
        {
            std::ifstream bmpfile(filename, std::ios::binary | std::ios::in);
            BITMAPINFOHEADER bih;
            BITMAPFILEHEADER bfh;
            bmpfile.read((char*)&bfh, sizeof(BITMAPFILEHEADER));
            bmpfile.read((char*)&bih, sizeof(BITMAPINFOHEADER));
            *this = TEXTURE_RGB(bih.biWidth, bih.biHeight);
            bmpfile.read((char*)buffer.get(), sizeX * sizeY * 3);
            bmpfile.close();
            for (unsigned int i = 0; i < sizeX * sizeY; i++)
                std::swap(buffer[i * 3], buffer[i * 3 + 2]);
        }
        void Save(const std::string& filename)
        {
            BITMAPFILEHEADER bitmapfileheader;
            BITMAPINFOHEADER bitmapinfoheader;
            bitmapfileheader.bfSize = sizeX * sizeY * 3 + 54;
            bitmapinfoheader.biWidth = sizeX;
            bitmapinfoheader.biHeight = sizeY;
            bitmapinfoheader.biSizeImage = sizeX * sizeY * 3;
            for (unsigned int i = 0; i != sizeX * sizeY * 3; i += 3)
                std::swap(buffer[i], buffer[i + 2]);
            std::ofstream ofs(filename, std::ios::out | std::ios::binary);
            ofs.write((char*)&bitmapfileheader, sizeof(bitmapfileheader));
            ofs.write((char*)&bitmapinfoheader, sizeof(bitmapinfoheader));
            ofs.write((char*)buffer.get(), sizeof(ubyte) * sizeX * sizeY * 3);
            ofs.close();
        }
    };

    struct TEXTURE_RGBA
    {
        unsigned int sizeX;
        unsigned int sizeY;
        unique_ptr<ubyte[]> buffer;
        TEXTURE_RGBA()
        {
        }
        TEXTURE_RGBA(unsigned int _sizeX, unsigned int _sizeY)
            :sizeX(_sizeX), sizeY(_sizeY), buffer(unique_ptr<ubyte[]>(new ubyte[_sizeX * _sizeY * 4]))
        {
        }
        TEXTURE_RGBA(const std::string& filename, const std::string& mask = "")
        {
            std::ifstream bmpfile(filename, std::ios::binary | std::ios::in), maskfile;
            BITMAPFILEHEADER bfh, mbfh;
            BITMAPINFOHEADER bih, mbih;
            if (mask != "")
            {
                maskfile.open(mask, std::ios::binary | std::ios::in);
                maskfile.read((char*)&mbfh, sizeof(BITMAPFILEHEADER));
                maskfile.read((char*)&mbih, sizeof(BITMAPINFOHEADER));
            }
            bmpfile.read((char*)&bfh, sizeof(BITMAPFILEHEADER));
            bmpfile.read((char*)&bih, sizeof(BITMAPINFOHEADER));
            *this = TEXTURE_RGBA(bih.biWidth, bih.biHeight);
            ubyte* rgb = new ubyte[sizeX * sizeY * 3], *a = new ubyte[sizeX * sizeY * 3];
            if (mask != "")
                maskfile.read((char*)a, sizeX * sizeY * 3);
            bmpfile.read((char*)rgb, sizeX * sizeY * 3);
            for (unsigned int i = 0; i < sizeX * sizeY; i++)
            {
                buffer[i << 2] = rgb[i * 3 + 2];
                buffer[(i << 2) ^ 1] = rgb[i * 3 + 1];
                buffer[(i << 2) + 2] = rgb[i * 3];
                buffer[(i << 2) + 3] = mask == "" ? 255 : 255 - a[i * 3];
            }
            delete[] a;
            delete[] rgb;
        }
    };

    enum BlockTextureID
    {
        ROCK, GRASS_TOP, GRASS_SIDE, DIRT, STONE, PLANK, WOOD_TOP, WOOD_SIDE, BEDROCK, LEAF,
        GLASS, WATER, LAVA, GLOWSTONE, SAND, CEMENT, ICE, COAL, IRON, TNT, UNKNOWN
    };

    enum
    {
        NULLBLOCK = 63
    };

    static ubyte getTextureIndex(block blockname, ubyte side);
    static double getTexcoordX(item item, ubyte side)
    {
        return isBlock(item) ? (getTextureIndex(item, side) & 7) / 8.0 : NULLBLOCK;
    }
    static double getTexcoordY(item item, ubyte side)
    {
        return isBlock(item) ? (getTextureIndex(item, side) >> 3) / 8.0 : NULLBLOCK;
    }

    static TextureID LoadRGBTexture(string Filename)
    {
        TEXTURE_RGB image(Filename);
        TextureID ret;
        glGenTextures(1, &ret);
        glBindTexture(GL_TEXTURE_2D, ret);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        Build2DMipmaps(GL_RGB, image.sizeX, image.sizeY, (int)log2(image.sizeX), image.buffer.get());
        return ret;
    }

    static TextureID LoadFontTexture(string Filename)
    {
        TEXTURE_RGB image(Filename);
        TEXTURE_RGBA Texture(image.sizeX, image.sizeY);
        for (size_t i = 0; i != image.sizeX * image.sizeY; i++)
        {
            Texture.buffer[i << 2] = Texture.buffer[(i << 2) ^ 1] = Texture.buffer[(i << 2) ^ 2] = 255;
            Texture.buffer[(i << 2) ^ 3] = 255 - image.buffer[3 * i];
        }
        TextureID ret;
        glGenTextures(1, &ret);
        glBindTexture(GL_TEXTURE_2D, ret);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture.sizeX, Texture.sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, Texture.buffer.get());
        return ret;
    }

    static TextureID LoadRGBATexture(string Filename, string MkFilename)
    {
        TextureID ret;
        TEXTURE_RGBA image(Filename, MkFilename);
        glGenTextures(1, &ret);
        glBindTexture(GL_TEXTURE_2D, ret);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        Build2DMipmaps(GL_RGBA, image.sizeX, image.sizeY, (int)log2(BLOCKTEXTURE_UNITSIZE), image.buffer.get());
        return ret;
    }

    static TextureID LoadBlock3DTexture(string Filename, string MkFilename);

    static void Build2DMipmaps(GLenum format, int w, int h, int level, const ubyte* src);
};

#endif
