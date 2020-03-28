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
#include "Game/World/Blocks.h"
#include "Game/World/world.h"
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

        Int3 tmp;
        for (tmp.X = 0; tmp.X < Chunk::Size(); ++tmp.X)
            for (tmp.Y = 0; tmp.Y < Chunk::Size(); ++tmp.Y)
                for (tmp.Z = 0; tmp.Z < Chunk::Size(); ++tmp.Z) {
                    Game::World::BlockData b = chunk->getBlock(tmp);
                    auto target = Game::World::Blocks::getInstance()[b.getID()].isTranslucent() ? &mVATranslucent : &mVAOpacity;
                    BlockRendererManager::render(*target, b.getID(), chunk, tmp);
                }
    }

    [[nodiscard]] const VertexArray& getVAOpacity() const noexcept { return mVAOpacity; }
    [[nodiscard]] const VertexArray& getVATranslucent() const noexcept { return mVATranslucent; }
private:
    VertexArray mVAOpacity{262144, VertexFormat(2, 1, 0, 3)};
    VertexArray mVATranslucent{262144, VertexFormat(2, 1, 0, 3)};
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
    void render(const Int3& c) const {
        if (!mBuffer.isEmpty()) {
            Renderer::translate(Float3(c * Chunk::Size()));
            Renderer::setMatrix();
            mBuffer.render();
            Renderer::translate(Float3(-c * Chunk::Size()));
        }
    }

    void renderTrans(const Int3& c) const {
        if (!mBufferTrans.isEmpty()) {
            Renderer::translate(Float3(c * Chunk::Size()));
            Renderer::setMatrix();
            mBufferTrans.render();
            Renderer::translate(Float3(-c * Chunk::Size()));
        }
    }

private:
    // Vertex buffer object
    VertexBuffer mBuffer, mBufferTrans;
};
