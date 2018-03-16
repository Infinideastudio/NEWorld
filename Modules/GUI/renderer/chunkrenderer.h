// 
// GUI: chunkrenderer.h
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

#include "blockrenderer.h"
#include "renderer.h"
#include "Common/Utility.h"
#include "Game/SyncService/world/nwblock.h"
#include "Game/Context/nwcontext.hpp"
#include "Game/SyncService/world/world.h"
#include "vertexarray.h"

/**
 * \brief It stores all the render data (VA) used to render a chunk.
 *        But it does not involve OpenGL operations so it can be
 *        safely called from all threads.
 */
class ChunkRenderData {
public:
    /**
     * \brief Generate the render data, namely VA, from a chunk.
     *        Does not involve OpenGL functions.
     * \param chunk the chunk to be rendered.
     */
    void generate(const Chunk* chunk) {
        // TODO: merge face rendering

        Vec3i tmp;
        for (tmp.x = 0; tmp.x < Chunk::Size(); ++tmp.x)
            for (tmp.y = 0; tmp.y < Chunk::Size(); ++tmp.y)
                for (tmp.z = 0; tmp.z < Chunk::Size(); ++tmp.z) {
                    BlockData b = chunk->getBlock(tmp);
                    auto target = context.blocks[b.getID()].isTranslucent() ? &mVATranslucent : &mVAOpacity;
                    BlockRendererManager::render(*target, b.getID(), chunk, tmp);
                }
    }

    const VertexArray& getVAOpacity() const noexcept { return mVAOpacity; }
    const VertexArray& getVATranslucent() const noexcept { return mVATranslucent; }
private:
    VertexArray mVAOpacity{262144, VertexFormat(2, 3, 0, 3)};
    VertexArray mVATranslucent{262144, VertexFormat(2, 3, 0, 3)};
};

/**
 * \brief The renderer that can be used to render directly. It includes
 *        VBO that we need to render. It can be generated from a
 *        ChunkRenderData
 */
class ChunkRenderer : public NonCopyable {
public:
    /**
     * \brief Generate VBO from VA. Note that this function will call
     *        OpenGL functions and thus can be only called from the
     *        main thread.
     * \param data The render data that will be used to generate VBO
     */
    ChunkRenderer(const ChunkRenderData& data) {
        mBuffer.update(data.getVAOpacity());
        mBufferTrans.update(data.getVATranslucent());
    }

    ChunkRenderer(ChunkRenderer&& rhs) noexcept:
        mBuffer(std::move(rhs.mBuffer)), mBufferTrans(std::move(rhs.mBufferTrans)) {}

    ChunkRenderer& operator=(ChunkRenderer&& rhs) noexcept {
        mBuffer = std::move(rhs.mBuffer);
        mBufferTrans = std::move(rhs.mBufferTrans);
        return *this;
    }

    // Draw call
    void render(const Vec3i& c) const {
        if (!mBuffer.isEmpty()) {
            Renderer::translate(Vec3f(c * Chunk::Size()));
            mBuffer.render();
            Renderer::translate(Vec3f(-c * Chunk::Size()));
        }
    }

    void renderTrans(const Vec3i& c) const {
        if (!mBufferTrans.isEmpty()) {
            Renderer::translate(Vec3f(c * Chunk::Size()));
            mBufferTrans.render();
            Renderer::translate(Vec3f(-c * Chunk::Size()));
        }
    }

private:
    // Vertex buffer object
    VertexBuffer mBuffer, mBufferTrans;
};
