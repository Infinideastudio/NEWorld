/*
 * NEWorld: An free game with similar rules to Minecraft.
 * Copyright (C) 2016 NEWorld Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PARTICLES_H
#define PARTICLES_H

#include "Definitions.h"
#include "Hitbox.h"

class Particles
{
public:
    struct Particle
    {
        bool exist = false;
        Vector3D<double> pos;
        Vector3D<float> sp;
        float psize, tcX, tcY;
        int lasts;
        Hitbox::AABB hb;
        void Update();
        void Render();
    };
    static vector<Particle> ptcs;
    static Vector3D<double> ppos;
    static void updateall();
    static void renderall(const Vector3D<double>& pos);
    static void throwParticle(block pt, float x, float y, float z, float xs, float ys, float zs, float psz, int last);
};

#endif
