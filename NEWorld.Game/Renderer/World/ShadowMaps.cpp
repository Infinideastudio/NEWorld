#include "ShadowMaps.h"
#include "Universe/World/World.h"
#include "Renderer/Renderer.h"

namespace ShadowMaps {
    void BuildShadowMap(
            WorldRenderer::ChunksRenderer &chunksRenderer,
            double xpos, double ypos, double zpos, double curtime
    ) {
        const auto cx = World::GetChunkPos(static_cast<int>(xpos)), cy = World::GetChunkPos(
                static_cast<int>(ypos)), cz = World::GetChunkPos(static_cast<int>(zpos));

        Renderer::StartShadowPass();
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnableVertexAttribArray(4);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_FOG);
        glDisable(GL_BLEND);
        const auto scale = 16.0f * sqrt(3.0f);
        const auto length = Renderer::shadowdist * scale;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-length, length, -length, length, -length, length);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotated(Renderer::sunlightXrot, 1.0, 0.0, 0.0);
        glRotated(Renderer::sunlightYrot, 0.0, 1.0, 0.0);

        auto frame = chunksRenderer.List({cx, cy, cz}, Renderer::shadowdist + 1, /*curtime,*/ false);
        frame.Render(xpos, ypos, zpos, 3);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glDisableVertexAttribArray(4);
        Renderer::EndShadowPass();

        glEnable(GL_FOG);
        glEnable(GL_BLEND);
    }

    void RenderShadowMap(
            WorldRenderer::ChunksRenderer &chunksRenderer,
            double xpos, double ypos, double zpos, double curtime
    ) {
        const auto cx = World::GetChunkPos(static_cast<int>(xpos)), cy = World::GetChunkPos(
                static_cast<int>(ypos)), cz = World::GetChunkPos(static_cast<int>(zpos));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnableVertexAttribArray(4);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_FOG);
        glDisable(GL_BLEND);
        const auto scale = 16.0f * sqrt(3.0f);
        const auto length = Renderer::shadowdist * scale;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-length, length, -length, length, -length, length);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotated(Renderer::sunlightXrot, 1.0, 0.0, 0.0);
        glRotated(Renderer::sunlightYrot, 0.0, 1.0, 0.0);

        auto frame = chunksRenderer.List({cx, cy, cz}, Renderer::shadowdist + 1, /*curtime,*/ false);
        frame.Render(xpos, ypos, zpos, 3);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glDisableVertexAttribArray(4);

        glEnable(GL_FOG);
        glEnable(GL_BLEND);
    }
}
