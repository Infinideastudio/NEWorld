#pragma once

#include "Definitions.h"
#include "Textures.h"
#include "Universe/World/World.h"
#include "Coro/Coro.h"

namespace WorldRenderer {
    const int delta[6][3] = {{1,  0,  0},
                             {-1, 0,  0},
                             {0,  1,  0},
                             {0,  -1, 0},
                             {0,  0,  1},
                             {0,  0,  -1}};


    struct RenderPair {
        VBOID Buffer{0};
        vtxCount Count{0};
    };

    struct ChunkRender {
        bool Built;
        bool Visiable;
        Int3 Position;
        double LoadAnim;
        RenderPair Renders[4];
        std::weak_ptr<World::Chunk> Ref;

        explicit ChunkRender(const std::shared_ptr<World::Chunk> &c) :
                Built{false}, Position(c->GetPosition()),
                LoadAnim(static_cast<float>(Position.Y) * 16.0f + 16.0f), Ref(c) {}

        bool CheckBuild(const std::shared_ptr<World::Chunk>& c);

        void Rebuild(const std::shared_ptr<World::Chunk> &c);
    };

    //深度模型的面 | Face in depth model
    struct QuadPrimitive_Depth {
        int x, y, z, length, direction;

        QuadPrimitive_Depth() : x(0), y(0), z(0), length(0), direction(0) {}
    };
}