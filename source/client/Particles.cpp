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

#include "Particles.h"
#include "World.h"
#include "Textures.h"

vector<Particles::Particle> Particles::ptcs;
Vector3D<double> Particles::ppos;

void Particles::updateall()
{
    for (vector<Particle>::iterator iter = ptcs.begin(); iter < ptcs.end();)
        if (iter->exist)
        {
            iter->Update();
            if (iter->lasts <= 0)
            {
                iter->exist = false;
                iter = ptcs.erase(iter);
            }
            else
                ++iter;
        }
}

void Particles::renderall(const Vector3D<double>& pos)
{
    ppos = pos;
    for (Particle it:ptcs)
        if (it.exist)
            it.Render();
}

void Particles::throwParticle(block pt, float x, float y, float z, float xs, float ys, float zs, float psz, int last)
{
    float tcX1 = (float)Textures::getTexcoordX(pt, 2), tcY1 = (float)Textures::getTexcoordY(pt, 2);
    Particle ptc;
    ptc.exist = true;
    ptc.pos = Vector3D<double>(x, y, z);
    ptc.sp = Vector3D<float>(xs, ys, zs);
    ptc.psize = psz;
    ptc.hb = Hitbox::AABB(x - psz, y - psz, z - psz, x + psz, y + psz, z + psz);
    ptc.lasts = last;
    ptc.tcX = tcX1 + (float)pRandGen->get_double_co()*((float)BLOCKTEXTURE_UNITSIZE / BLOCKTEXTURE_SIZE)*(1.0f - psz);
    ptc.tcY = tcY1 + (float)pRandGen->get_double_co()*((float)BLOCKTEXTURE_UNITSIZE / BLOCKTEXTURE_SIZE)*(1.0f - psz);
    ptcs.push_back(ptc);
}

void Particles::Particle::Update()
{
    float psz = psize;

    hb = Hitbox::AABB(pos.x - psz, pos.y - psz, pos.z - psz, pos.x + psz, pos.y + psz, pos.z + psz);
    double dx = sp.x, dy = sp.y, dz = sp.z;

    vector<Hitbox::AABB> Hitboxes = World::getHitboxes(Hitbox::Expand(hb, dx, dy, dz));
    for (size_t i = 0; i < Hitboxes.size(); i++)
    {
        dy = Hitbox::MaxMoveOnYclip(hb, Hitboxes[i], dy);
    }
    Hitbox::Move(hb, 0.0, dy, 0.0);
    for (size_t i = 0; i < Hitboxes.size(); i++)
    {
        dx = Hitbox::MaxMoveOnXclip(hb, Hitboxes[i], dx);
    }
    Hitbox::Move(hb, dx, 0.0, 0.0);
    for (size_t i = 0; i < Hitboxes.size(); i++)
    {
        dz = Hitbox::MaxMoveOnZclip(hb, Hitboxes[i], dz);
    }
    Hitbox::Move(hb, 0.0, 0.0, dz);

    pos += Vector3D<double>(dx, dy, dz);
    if (dy != sp.y) sp.y = 0.0;
    sp.x *= 0.6f;
    sp.y *= 0.6f;
    sp.z -= 0.01f;
    lasts -= 1;
}

void Particles::Particle::Render()
{
    //if (!Frustum::aabbInFrustum(hb)) return;
    float size = (float)BLOCKTEXTURE_UNITSIZE / BLOCKTEXTURE_SIZE * (psize <= 1.0f ? psize : 1.0f);
    float col = World::getbrightness(RoundInt(pos.x), RoundInt(pos.y), RoundInt(pos.z)) / (float)World::BRIGHTNESSMAX;
    float col1 = col * 0.5f;
    float col2 = col * 0.7f;
    double palpha = (lasts < 30 ? lasts / 30.0 : 1.0);
    double xpos = this->pos.x - ppos.x;
    double ypos = this->pos.y - ppos.y;
    double zpos = this->pos.z - ppos.z;

    glBegin(GL_QUADS);
    glColor4d(col1, col1, col1, palpha);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 0.0);
    glVertex3d(xpos - psize, ypos - psize, zpos + psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 0.0);
    glVertex3d(xpos + psize, ypos - psize, zpos + psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 1.0);
    glVertex3d(xpos + psize, ypos + psize, zpos + psize);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 1.0);
    glVertex3d(xpos - psize, ypos + psize, zpos + psize);

    glColor4d(col1, col1, col1, palpha);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 0.0);
    glVertex3d(xpos - psize, ypos + psize, zpos - psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 0.0);
    glVertex3d(xpos + psize, ypos + psize, zpos - psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 1.0);
    glVertex3d(xpos + psize, ypos - psize, zpos - psize);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 1.0);
    glVertex3d(xpos - psize, ypos - psize, zpos - psize);

    glColor4d(col, col, col, palpha);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 0.0);
    glVertex3d(xpos + psize, ypos + psize, zpos - psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 0.0);
    glVertex3d(xpos - psize, ypos + psize, zpos - psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 1.0);
    glVertex3d(xpos - psize, ypos + psize, zpos + psize);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 1.0);
    glVertex3d(xpos + psize, ypos + psize, zpos + psize);

    glColor4d(col, col, col, palpha);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 0.0);
    glVertex3d(xpos - psize, ypos - psize, zpos - psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 0.0);
    glVertex3d(xpos + psize, ypos - psize, zpos - psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 1.0);
    glVertex3d(xpos + psize, ypos - psize, zpos + psize);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 1.0);
    glVertex3d(xpos - psize, ypos - psize, zpos + psize);

    glColor4d(col2, col2, col2, palpha);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 0.0);
    glVertex3d(xpos + psize, ypos + psize, zpos - psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 0.0);
    glVertex3d(xpos + psize, ypos + psize, zpos + psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 1.0);
    glVertex3d(xpos + psize, ypos - psize, zpos + psize);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 1.0);
    glVertex3d(xpos + psize, ypos - psize, zpos - psize);

    glColor4d(col2, col2, col2, palpha);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 0.0);
    glVertex3d(xpos - psize, ypos - psize, zpos - psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 0.0);
    glVertex3d(xpos - psize, ypos - psize, zpos + psize);
    glTexCoord2d(tcX + size * 1.0, tcY + size * 1.0);
    glVertex3d(xpos - psize, ypos + psize, zpos + psize);
    glTexCoord2d(tcX + size * 0.0, tcY + size * 1.0);
    glVertex3d(xpos - psize, ypos + psize, zpos - psize);
    glEnd();
}
