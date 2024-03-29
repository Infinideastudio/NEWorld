#pragma once

#include "Renderer/Renderer.h"
#include "ChunkRenderer.h"
#include <kls/temp/STL.h>
#include <kls/thread/SpinLock.h>

namespace WorldRenderer {
    class FrameChunksRenderer {
    public:
        explicit FrameChunksRenderer(
                const std::vector<ChunkRender> &list,
                Int3 cPos, int renderDist, bool frus = true
        );

        void Render(double x, double y, double z, int buffer);

    private:
        kls::temp::vector<const ChunkRender *> mFiltered;
    };

    class ChunksRenderer {
    public:
        kls::coroutine::ValueAsync<void> Update(Int3 position);

        void FrustumUpdate(Double3 camera, Frustum &frus);

        void PurgeTable(int invalidated);

        auto List(Int3 cPos, int renderDist, bool frus = true) {
            return FrameChunksRenderer(mChunks, cPos, renderDist, frus);
        }

        void Add(const std::shared_ptr<World::Chunk> &c) {
            std::lock_guard lk{mMAdd};
            mLAdd.emplace_back(c);
        }

        // TODO(Try to remove this)
        static ChunksRenderer &Default() {
            static ChunksRenderer instance{};
            return instance;
        }

    private:
        kls::thread::SpinLock mMAdd;
        std::vector<ChunkRender> mLAdd;
        std::vector<ChunkRender> mChunks;
    };

    extern int chunkBuildRenders;
}