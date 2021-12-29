#pragma once

#include "Renderer/Renderer.h"
#include "ChunkRenderer.h"
#include <Temp/Vector.h>

namespace WorldRenderer {
    class FrameChunksRenderer {
    public:
        explicit FrameChunksRenderer(
                const std::vector<ChunkRender> &list,
                Int3 cPos, int renderDist, bool frus = true
        );

        void Render(double x, double y, double z, int buffer);
    private:
        temp::vector<const ChunkRender *> mFiltered;
    };

    class ChunksRenderer {
    public:
        void Update(Int3 position, Double3 camera, Frustum &frus);

        auto List(Int3 cPos, int renderDist, bool frus = true) {
            return FrameChunksRenderer(mChunks, cPos, renderDist, frus);
        }

        void Add(const std::shared_ptr<World::Chunk>& c) {
            mChunks.emplace_back(c);
        }

        // TODO(Try to remove this)
        static ChunksRenderer& Default() {
            static ChunksRenderer instance{};
            return instance;
        }
    private:
        std::vector<ChunkRender> mChunks;
    };

    extern int chunkBuildRenders;
}