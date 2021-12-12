#include "WorldRenderer.h"

namespace WorldRenderer {
    std::vector<RenderChunk> RenderChunkList;

    int ListRenderChunks(int cx, int cy, int cz, int renderdistance, double curtime, bool frustest) {
        const auto cPos = Int3{cx, cy, cz};
        auto renderedChunks = 0;
        RenderChunkList.clear();
        for (auto& chunk : World::chunks) {
            if (!chunk->renderBuilt || chunk->Empty) continue;
            if (ChebyshevDistance(cPos, chunk->GetPosition()) <= renderdistance) {
                if (!frustest || chunk->visible) {
                    renderedChunks++;
                    RenderChunkList.emplace_back(chunk, (curtime - lastupdate) * 30.0);
                }
            }
        }
        return renderedChunks;
    }

    void RenderChunks(double x, double y, double z, int buffer) {
        auto TexcoordCount = 2, ColorCount = 3;
        float m[16];
        if (buffer != 3) {
            memset(m, 0, sizeof(m));
            m[0] = m[5] = m[10] = m[15] = 1.0f;
        } else TexcoordCount = ColorCount = 0;

        for (auto cr : RenderChunkList) {
            if (cr.vertexes[0] == 0) continue;
            glPushMatrix();
            glTranslated(cr.cx * 16.0 - x, cr.cy * 16.0 - cr.loadAnim - y, cr.cz * 16.0 - z);
            if (Renderer::AdvancedRender && buffer != 3) {
                m[12] = cr.cx * 16.0f - static_cast<float>(x);
                m[13] = cr.cy * 16.0f - static_cast<float>(cr.loadAnim) - static_cast<float>(y);
                m[14] = cr.cz * 16.0f - static_cast<float>(z);
                Renderer::shaders[Renderer::ActiveShader].setUniform("TransMat", m);
                Renderer::RenderBufferDirect(cr.vbuffers[buffer], cr.vertexes[buffer], TexcoordCount, ColorCount, 1);
            } else Renderer::RenderBufferDirect(cr.vbuffers[buffer], cr.vertexes[buffer], TexcoordCount, ColorCount);
            glPopMatrix();
        }

        glFlush();
    }
}