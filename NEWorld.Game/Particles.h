#pragma once

#include "Definitions.h"
#include "Universe/Entity/bvh.h"

namespace Particles {
    const int PARTICALE_MAX = 4096;
    struct Particle {
        bool exist = false;
        double xpos, ypos, zpos;
        float xsp, ysp, zsp, psize, tcX, tcY;
        int lasts;
        BoundingBox hb;
    };
    extern std::vector<Particle> ptcs;
    extern int ptcsrendered;

    void update(Particle &ptc);

    void updateall();

    void renderall(double xpos, double ypos, double zpos);

    void throwParticle(Block pt, float x, float y, float z, float xs, float ys, float zs, float psz, int last);

    inline void throwParticle(Block pt, Int3 pos) {
        throwParticle(
            pt,
            float(pos.X + rnd() - 0.5f), float(pos.Y + rnd() - 0.2f),
            float(pos.Z + rnd() - 0.5f),
            float(rnd() * 0.2f - 0.1f), float(rnd() * 0.2f - 0.1f),
            float(rnd() * 0.2f - 0.1f),
            float(rnd() * 0.01f + 0.02f), int(rnd() * 30) + 30
        );
    }
}
