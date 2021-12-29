#include "WorldRenderer.h"
#include <queue>

namespace WorldRenderer {
    int MaxChunkRenders = 4;
    int chunkBuildRenders;

    static constexpr auto ccOffset = Int3(7); // offset to a chunk center

    // TODO(make it better, the function is bad)
    void ChunksRenderer::Update(Int3 position, Double3 camera, Frustum &frus) {
        struct SortEntry {
            int Distance;
            ChunkRender *Render;
            std::shared_ptr<World::Chunk> Locked;
        };
        struct SortCmp {
            [[nodiscard]] constexpr bool operator()(const SortEntry &l, const SortEntry &r) const {
                return l.Distance > r.Distance;
            }
        };
        // Sort the update priority list, also update the frustum results
        int invalidated = 0;
        World::Chunk::setRelativeBase(camera.X, camera.Y, camera.Z);
        {
            const auto cp = World::GetChunkPos(position);
            std::priority_queue<SortEntry, temp::vector<SortEntry>, SortCmp> candidate;
            for (auto &entry: mChunks) {
                if (auto lock = entry.Ref.lock(); lock) {
                    entry.Visiable = frus.FrustumTest(lock->getRelativeAABB());
                    if (!lock->updated) continue;
                    const auto c = lock->GetPosition();
                    if (ChebyshevDistance(c, cp) > viewdistance) continue;
                    const auto distance = static_cast<int>(DistanceSquared(c * 16 + ccOffset, position));
                    candidate.push(SortEntry{distance, &entry, std::move(lock)});
                } else ++invalidated;
            }
            // walk the candidates and update max elements
            int released = 0;
            while ((released < MaxChunkRenders) && (!candidate.empty())) {
                auto &top = candidate.top();
                if (top.Render->TryRebuild(top.Locked)) ++released;
                candidate.pop();
            }
            chunkBuildRenders = released;
        }
        // purge the table if there are too many dead items.
        temp::vector<VBOID> toRelease;
        if (invalidated * 4 > mChunks.size()) {
            std::vector<ChunkRender> swap;
            for (auto &entry: mChunks) {
                if (!entry.Ref.expired()) {
                    swap.push_back(std::move(entry));
                } else if (entry.Built) {
                    if (entry.Renders[0].Buffer != 0) toRelease.push_back(entry.Renders[0].Buffer);
                    if (entry.Renders[1].Buffer != 0) toRelease.push_back(entry.Renders[1].Buffer);
                    if (entry.Renders[2].Buffer != 0) toRelease.push_back(entry.Renders[2].Buffer);
                    if (entry.Renders[3].Buffer != 0) toRelease.push_back(entry.Renders[3].Buffer);
                }
            }
            mChunks.swap(swap);
        }
        if (!toRelease.empty()) glDeleteBuffers(toRelease.size(), toRelease.data());
    }

    FrameChunksRenderer::FrameChunksRenderer(
            const std::vector<ChunkRender> &list,
            Int3 cPos, int renderDist, bool frus
    ) {
        auto renderedChunks = 0;
        for (auto &entry: list) {
            if (!entry.Built) continue;
            if (ChebyshevDistance(cPos, entry.Position) <= renderDist) {
                if (!frus || entry.Visiable) {
                    renderedChunks++;
                    mFiltered.push_back(&entry);
                    //RenderChunkList.emplace_back(chunk.get(), (curtime - lastUpdate) * 30.0);
                    // position(c->GetPosition()), loadAnim(c->loadAnim * pow(0.6, TimeDelta)) {
                }
            }
        }
    }

    void FrameChunksRenderer::Render(double x, double y, double z, int buffer) {
        float m[16];
        if (buffer != 3) {
            memset(m, 0, sizeof(m));
            m[0] = m[5] = m[10] = m[15] = 1.0f;
        }
        for (auto cr: mFiltered) {
            if (cr->Renders[buffer].Count == 0) continue;
            const auto offset = Double3(cr->Position) * 16.0 - Double3(x, /*cr.loadAnim*/ +y, z);
            glPushMatrix();
            glTranslated(offset.X, offset.Y, offset.Z);
            if (Renderer::AdvancedRender && buffer != 3) {
                m[12] = static_cast<float>(offset.X);
                m[13] = static_cast<float>(offset.Y);
                m[14] = static_cast<float>(offset.Z);
                Renderer::GetPipeline()->SetUniform(4, m);
            }
            Renderer::RenderBufferDirect(cr->Renders[buffer].Buffer, cr->Renders[buffer].Count);
            glPopMatrix();
        }

        glFlush();
    }
}