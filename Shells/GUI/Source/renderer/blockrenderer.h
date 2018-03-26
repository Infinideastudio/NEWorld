// 
// GUI: blockrenderer.h
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

#include <vector>
#include <memory>
#include "texture.h"
#include "Common/Math/Vector.h"

class VertexArray;

struct BlockTexCoord {
    size_t pos = 0;
    float d[4] = {0.0f, 0.0f, 0.0f, 0.0f};
};

class BlockRenderer {
public:
    virtual ~BlockRenderer() = default;

    virtual void flushTexture() = 0;
    virtual void render(VertexArray& target, const class Chunk* chunk, const Vec3i& pos) = 0;
};

class DefaultBlockRenderer : public BlockRenderer {
public:
    DefaultBlockRenderer(size_t data[]);
    void flushTexture() override;
    void render(VertexArray& target, const class Chunk* chunk, const Vec3i& pos) override;
private:
    BlockTexCoord tex[6];
};

class BlockTextureBuilder {
public:
    static size_t capacity();
    static size_t capacityRaw();
    static void setWidthPerTex(size_t wid);
    static size_t getWidthPerTex();
    static size_t addTexture(const char* path);
    static size_t addTexture(const Texture::RawTexture& rawTexture);

    //Finalize Func
    static Texture buildAndFlush();

    //In
    static size_t getTexturePerLine();
    static void getTexturePos(float pos[], size_t id);

private:
    static size_t mPixelPerTexture, mTexturePerLine;
    static std::vector<Texture::RawTexture> mRawTexs;
};

class BlockRendererManager {
public:
    static void render(VertexArray& target, size_t id, const class Chunk* chunk, const Vec3i& pos); //RenderList
    static void setBlockRenderer(size_t pos, std::shared_ptr<BlockRenderer>&& blockRenderer);
    static void flushTextures();
private:
    static std::vector<std::shared_ptr<BlockRenderer>> mBlockRenderers;
};
