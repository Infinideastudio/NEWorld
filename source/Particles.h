#pragma once
#include "Definitions.h"
#include "Hitbox.h"

class Particles
{
public:
    const int PARTICALE_MAX = 4096;
    struct Particle
    {
        bool exist = false;
        double xpos, ypos, zpos;
        float xsp, ysp, zsp, psize, tcX, tcY;
        int lasts;
        Hitbox::AABB hb;
    };
    static vector<Particle> ptcs;
    static int ptcsrendered;
    static double pxpos, pypos, pzpos;
    static void update(Particle &ptc);
    static void updateall();
    static void render(Particle &ptc);
    static void renderall(double xpos, double ypos, double zpos);
    static void throwParticle(block pt, float x, float y, float z, float xs, float ys, float zs, float psz, int last);
};
