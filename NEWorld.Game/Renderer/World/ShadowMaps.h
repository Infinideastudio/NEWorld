#pragma once

#include "Renderer/World/WorldRenderer.h"

namespace ShadowMaps {
    void BuildShadowMap(
            WorldRenderer::ChunksRenderer &chunksRenderer,
            double xpos, double ypos, double zpos, double curtime
    );

    void RenderShadowMap(
            WorldRenderer::ChunksRenderer &chunksRenderer,
            double xpos, double ypos, double zpos, double curtime
    );
    //void BuildCascadedShadowMaps(double xpos, double ypos, double zpos, double curtime);
    //void DrawShadowMap(int xi, int yi, int xa, int ya);
}