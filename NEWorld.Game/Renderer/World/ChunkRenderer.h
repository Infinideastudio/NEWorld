#pragma once

#include "Definitions.h"
#include "Textures.h"
#include "Universe/World/World.h"
#include "Coro/Coro.h"

namespace WorldRenderer {
    const int delta[6][3] = { {1,  0,  0},
                             {-1, 0,  0},
                             {0,  1,  0},
                             {0,  -1, 0},
                             {0,  0,  1},
                             {0,  0,  -1} };


    struct RenderPair {
        VBOID Buffer{ 0 };
        vtxCount Count{ 0 };
    };

    struct ChunkRender {
        bool Built;
        bool Visiable;
        Int3 Position;
        double LoadAnim;
        RenderPair Renders[4];
        BoundingBox BaseBounds;
        std::weak_ptr<World::Chunk> Ref;

        explicit ChunkRender(const std::shared_ptr<World::Chunk>& c) :
            Built{ false }, Position(c->GetPosition()),
            LoadAnim(static_cast<float>(Position.Y) * 16.0f + 16.0f),
            BaseBounds(c->getBaseAABB()), Ref(c) {}

        bool CheckBuild(const std::shared_ptr<World::Chunk>& c);

        ValueAsync<void> Rebuild(std::shared_ptr<World::Chunk> c);

        Frustum::ChunkBox getRelativeAABB(const Double3& camera) {
            Frustum::ChunkBox ret{};
            ret.xmin = static_cast<float>(BaseBounds.min.values[0] - camera.X);
            ret.xmax = static_cast<float>(BaseBounds.max.values[0] - camera.X);
            ret.ymin = static_cast<float>(BaseBounds.min.values[1] /*- loadAnim*/ - camera.Y);
            ret.ymax = static_cast<float>(BaseBounds.max.values[1] /*- loadAnim*/ - camera.Y);
            ret.zmin = static_cast<float>(BaseBounds.min.values[2] - camera.Z);
            ret.zmax = static_cast<float>(BaseBounds.max.values[2] - camera.Z);
            return ret;
        }
    };

    //深度模型的面 | Face in depth model
    struct QuadPrimitive_Depth {
        int x, y, z, length, direction;

        QuadPrimitive_Depth() : x(0), y(0), z(0), length(0), direction(0) {}
    };
}
