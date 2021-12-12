#pragma once

#include "Definitions.h"
#include "Shader.h"
#include <cstring>

namespace Renderer {
    //我猜你肯定不敢看Renderer.cpp  --qiaozhanrong
    //猜对了  --Null

    enum {
        MainShader, ShadowShader, DepthShader
    };

    const int ArraySize = 2621440;
    extern float *VA;
    extern int size;
    extern int Vertexes;
    extern bool AdvancedRender;
    extern int ShadowRes;
    extern int MaxShadowDist;
    extern int shadowdist;
    extern float sunlightXrot, sunlightYrot;
    extern unsigned int DepthTexture;
    extern std::vector<Shader> shaders;
    extern int ActiveShader;

    void Init(int tc, int cc, int ac = 0);

    template <int count, class... Elem>
    inline void Batch(Elem... elem) noexcept {
        std::initializer_list<float> v {static_cast<float>(std::forward<Elem>(elem))...};
        VA = std::copy(v.begin(), v.end(), VA);
        Vertexes += count;
    }

    void Flush(VBOID &buffer, vtxCount &vtxs);

    void BatchStart(int tc, int cc, int ac) noexcept;

    void RenderBufferDirect(VBOID buffer, vtxCount vtxs, int tc, int cc, int ac = 0);

    void initShaders();

    inline void bindShader(int shaderID) {
        shaders[shaderID].bind();
        ActiveShader = shaderID;
    }

    void destroyShaders();

    void EnableShaders();

    void DisableShaders();

    void StartShadowPass();

    void EndShadowPass();
}
