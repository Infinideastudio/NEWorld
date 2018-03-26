// 
// GUI: texture.h
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#pragma once

#include <string>
#include "opengl.h"

using TextureID = GLuint;
using ubyte = unsigned char;
struct SDL_Surface;

class Texture {
public:
    enum {
        Texture2D = GL_TEXTURE_2D,
        Texture3D = GL_TEXTURE_3D
    };

    Texture() = default;

    explicit Texture(TextureID id) : mId(id) {}

    static void init();

    static void free();

    static Texture loadTextureRGB(std::string filename);

    static Texture loadTextureRGBA(std::string filename);

    static Texture loadBlock3DTexture(std::string filename);

    TextureID getTextureID() const { return mId; }

    void bind(GLenum target) const { glBindTexture(target, mId); }

    friend class BlockTextureBuilder;

private:
    TextureID mId;

    class RawTexture {
    public:
        ~RawTexture();
        explicit RawTexture(const std::string& filename);
        RawTexture(const RawTexture&);
        SDL_Surface* getSurface() const { return surface; }
    private:
        SDL_Surface* surface;
    };

    static void build2DMipmaps(GLenum format, int w, int h, int level, const ubyte* src);
};
