/*
* NEWorld: A free game with similar rules to Minecraft.
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

#include "texture.h"
#include <SDL2/SDL_image.h>
#include <logger.h>
#include <memory>
#include <sstream>
#include <iomanip>

constexpr int align(int x, int al)
{
    return(x%al == 0 ? x : (x/al + 1) * al);
}

void Texture::init()
{
    // We don't need JPEG...
    if (!IMG_Init(/*IMG_INIT_JPG | */IMG_INIT_PNG))
    {
        errorstream << "Failed to initialize PNG support! " << IMG_GetError();
    }
}

void Texture::uninit()
{
    IMG_Quit();
}

Texture Texture::loadTexture(std::string filename)
{
    RawTexture image(filename);
    auto surf = image.getSurface();
    TextureID ret;
    glGenTextures(1, &ret);
    glBindTexture(GL_TEXTURE_2D, ret);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    build2DMipmaps(GL_RGB, surf->w, surf->h, static_cast<int>(log2(surf->w)), static_cast<const ubyte*>(surf->pixels));
    return Texture(ret);
}

Texture Texture::loadBlock3DTexture(std::string filename)
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
    RawTexture image(filename);
    auto surf = image.getSurface();
    //TODO: fix it
    //glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, BLOCKTEXTURE_UNITSIZE, BLOCKTEXTURE_UNITSIZE, BLOCKTEXTURE_UNITS * BLOCKTEXTURE_UNITS, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<const ubyte*>(surf->pixels));
    glDisable(GL_TEXTURE_3D);
    glEnable(GL_TEXTURE_2D);
    return Texture(ret);
}

void Texture::build2DMipmaps(GLenum format, int w, int h, int level, const ubyte* src)
{
    size_t sum, scale = 1, cc = 0;
    if (format == GL_RGBA) cc = 4;
    else if (format == GL_RGB) cc = 3;
    std::unique_ptr<ubyte[]> cur(new ubyte[w*h*cc]);
    memset(cur.get(), 0, w*h*cc);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, level);
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, 0.0f);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, src);
    for (int i = 1; i <= level; i++)
    {
        scale *= 2;
        size_t cur_w = w / scale, cur_h = h / scale;
        for (size_t y = 0; y < cur_h; y++)
            for (size_t x = 0; x < cur_w; x++)
                for (size_t col = 0; col < cc; col++)
                {
                    sum = 0;
                    for (size_t yy = 0; yy < scale; yy++)
                        for (size_t xx = 0; xx < scale; xx++)
                            sum += src[(y * scale + yy) * align(w*cc, 4) + (x * scale + xx) * cc + col];
                    cur[y * align(cur_w*cc, 4) + x * cc + col] = static_cast<ubyte>(sum / (scale*scale));
                }
        glTexImage2D(GL_TEXTURE_2D, i, format, cur_w, cur_h, 0, format, GL_UNSIGNED_BYTE, cur.get());
    }
}

Texture::RawTexture::RawTexture(const std::string& filename)
{
    auto image = IMG_Load(filename.c_str());
    if (!image) warningstream << "Failed to load texture " << filename << ": " << IMG_GetError();
    surface = image;
    // TODO: check if format = B8G8R8
    // if (image->format == )
    for(int i=0; i<surface->h; i++)
    {
        for(int j=0; j<surface->w; j++)
        {
            unsigned char* p = static_cast<unsigned char*>(surface->pixels) + (i*surface->w + j) * 3;
            unsigned char t = *p;
            *p = *(p + 2);
            *(p + 2) = t;
        }
    }
    // TODO: add support for B8G8R8A8 format
}

Texture::RawTexture::~RawTexture()
{
    SDL_FreeSurface(surface);
}
