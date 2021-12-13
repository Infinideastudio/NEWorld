#pragma once

#include "Definitions.h"
#include "GL/Pipeline.h"
#include <cstring>

namespace Renderer {
    //我猜你肯定不敢看Renderer.cpp  --qiaozhanrong
    //猜对了  --Null

    enum {
        MainShader, ShadowShader, DepthShader, SimpleShader
    };

    const int ArraySize = 2621440;
    extern bool AdvancedRender;
    extern int ShadowRes;
    extern int MaxShadowDist;
    extern int shadowdist;
    extern float sunlightXrot, sunlightYrot;
    extern unsigned int DepthTexture;
    extern int ActivePipeline;

    void BatchStart(int tc, int cc, int ac) noexcept;

    void RenderBufferDirect(VBOID buffer, vtxCount vtxs);

    void initShaders();

    void destroyShaders();

    void EnableShaders();

    void DisableShaders();

    void StartShadowPass();

    void EndShadowPass();
}
