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

#ifndef TEXTURES_H
#define TEXTURES_H
#include <string>
#include "opengl.h"

using TextureID = GLuint;
using ubyte = unsigned char;
class SDL_Surface;

class Texture
{
public:
    static void init();

    static void uninit();

    static Texture loadTexture(std::string filename);

    static Texture loadBlock3DTexture(std::string filename);

    TextureID getTextureID() const { return m_id; }

private:

    explicit Texture(TextureID id) :m_id(id) {}

    TextureID m_id;

    static void build2DMipmaps(GLenum format, int w, int h, int level, const ubyte* src);

    class RawTexture
    {
    public:
        ~RawTexture();
        explicit RawTexture(const std::string& filename);
        const SDL_Surface* getSurface() const { return surface; }

    private:
        SDL_Surface* surface;
    };

};

#endif
