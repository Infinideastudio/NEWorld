#pragma once

#include "Definitions.h"
#include "Textures.h"

namespace World { class Chunk; }

namespace ChunkRenderer {
    const int delta[6][3] = {{1,  0,  0},
                             {-1, 0,  0},
                             {0,  1,  0},
                             {0,  -1, 0},
                             {0,  0,  1},
                             {0,  0,  -1}};

    //深度模型的面 | Face in depth model
    struct QuadPrimitive_Depth {
        int x, y, z, length, direction;

        QuadPrimitive_Depth() : x(0), y(0), z(0), length(0), direction(0) {}
    };

    void RenderPrimitive_Depth(QuadPrimitive_Depth &p);

    void RenderChunk(World::Chunk *c);

    void RenderDepthModel(World::Chunk *c);

}