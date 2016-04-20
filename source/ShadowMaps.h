#ifndef SHADOWMAPS_H
#define SHADOWMAPS_H

#include "WorldRenderer.h"
#include "Renderer.h"

namespace ShadowMaps
{
void BuildShadowMap(double xpos, double ypos, double zpos, double curtime);
void RenderShadowMap(double xpos, double ypos, double zpos, double curtime);
//void BuildCascadedShadowMaps(double xpos, double ypos, double zpos, double curtime);
//void DrawShadowMap(int xi, int yi, int xa, int ya);
}

#endif
