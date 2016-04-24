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

#include "Hitbox.h"
#include "Definitions.h"

namespace Hitbox
{

bool stuck = false;

bool inXclip(const AABB& boxA, const AABB& boxB)
{
    if ((boxA.xmin > boxB.xmin && boxA.xmin < boxB.xmax) || (boxA.xmax > boxB.xmin && boxA.xmax < boxB.xmax) || 
        (boxB.xmin > boxA.xmin && boxB.xmin < boxA.xmax) || (boxB.xmax > boxA.xmin && boxB.xmax < boxA.xmax)) return true;
    return false;
}

bool inYclip(const AABB& boxA, const AABB& boxB)
{
    if ((boxA.ymin > boxB.ymin && boxA.ymin < boxB.ymax) || (boxA.ymax > boxB.ymin && boxA.ymax < boxB.ymax) ||
        (boxB.ymin > boxA.ymin && boxB.ymin < boxA.ymax) || (boxB.ymax > boxA.ymin && boxB.ymax < boxA.ymax)) return true;
    return false;
}

bool inZclip(const AABB& boxA, const AABB& boxB)
{
    if ((boxA.zmin > boxB.zmin && boxA.zmin < boxB.zmax) || (boxA.zmax > boxB.zmin && boxA.zmax < boxB.zmax) ||
        (boxB.zmin > boxA.zmin && boxB.zmin < boxA.zmax) || (boxB.zmax > boxA.zmin && boxB.zmax < boxA.zmax)) return true;
    return false;
}

bool Hit(const AABB& boxA, const AABB& boxB)
{
    return inXclip(boxA, boxB) && inYclip(boxA, boxB) && inZclip(boxA, boxB);
}

double MaxMoveOnXclip(const AABB& boxA, const AABB& boxB, double movedist)
{
    //用boxA去撞boxB
    if (!(inYclip(boxA, boxB) && inZclip(boxA, boxB))) return movedist;
    else if (boxA.xmin >= boxB.xmax && movedist < 0.0) return max(boxB.xmax - boxA.xmin, movedist);
    else if (boxA.xmax <= boxB.xmin && movedist > 0.0) return min(boxB.xmin - boxA.xmax, movedist);
    //assert(false);
    return !stuck ? movedist : 0.0;
}

double MaxMoveOnYclip(const AABB& boxA, const AABB& boxB, double movedist)
{
    //用boxA去撞boxB
    if (!(inXclip(boxA, boxB) && inZclip(boxA, boxB))) return movedist;
    else if (boxA.ymin >= boxB.ymax && movedist < 0.0) return max(boxB.ymax - boxA.ymin, movedist);
    else if (boxA.ymax <= boxB.ymin && movedist > 0.0) return min(boxB.ymin - boxA.ymax, movedist);
    //assert(false);
    return !stuck ? movedist : 0.0;
}

double MaxMoveOnZclip(const AABB& boxA, const AABB& boxB, double movedist)
{
    //用boxA去撞boxB
    if (!(inXclip(boxA, boxB) && inYclip(boxA, boxB))) return movedist;
    else if (boxA.zmin >= boxB.zmax && movedist < 0.0) return max(boxB.zmax - boxA.zmin, movedist);
    else if (boxA.zmax <= boxB.zmin && movedist > 0.0) return min(boxB.zmin - boxA.zmax, movedist);
    //assert(false);
    return !stuck ? movedist : 0.0;
}

AABB Expand(const AABB& box, double xe, double ye, double ze)
{
    AABB ret = box;
    if (xe > 0.0) ret.xmax += xe;
    else ret.xmin += xe;
    if (ye > 0.0) ret.ymax += ye;
    else ret.ymin += ye;
    if (ze > 0.0) ret.zmax += ze;
    else ret.zmin += ze;
    return ret;
}

void Move(AABB &box, double xa, double ya, double za)
{
    box.xmin += xa;
    box.ymin += ya;
    box.zmin += za;
    box.xmax += xa;
    box.ymax += ya;
    box.zmax += za;
}

void MoveTo(AABB &box, double x, double y, double z)
{
    double l, w, h;
    //注意在执行这个过程时，参数中的xyz坐标将成为移动后的AABB的中心，而不是初始化AABB时的原点！
    l = (box.xmax - box.xmin) / 2;
    w = (box.ymax - box.ymin) / 2;
    h = (box.zmax - box.zmin) / 2;
    box.xmin = x - l;
    box.ymin = y - w;
    box.zmin = z - h;
    box.xmax = x + l;
    box.ymax = y + w;
    box.zmax = z + h;
}
void renderAABB(const AABB& box, float colR, float colG, float colB, int mode, double EPS)
{
    //Debug only!
    //碰撞箱渲染出来很瞎狗眼的QAQ而且又没用QAQ

    glLineWidth(2.0);
    glEnable(GL_LINE_SMOOTH);
    glColor4f(colR, colG, colB, 1.0);

    if (mode == 1 || mode == 3)
    {
        glBegin(GL_LINE_LOOP);
        glVertex3d(box.xmin, box.ymin, box.zmin);
        glVertex3d(box.xmax, box.ymin, box.zmin);
        glVertex3d(box.xmax, box.ymax, box.zmin);
        glVertex3d(box.xmin, box.ymax, box.zmin);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex3d(box.xmin, box.ymin, box.zmax);
        glVertex3d(box.xmax, box.ymin, box.zmax);
        glVertex3d(box.xmax, box.ymax, box.zmax);
        glVertex3d(box.xmin, box.ymax, box.zmax);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex3d(box.xmin, box.ymin, box.zmin);
        glVertex3d(box.xmax, box.ymin, box.zmin);
        glVertex3d(box.xmax, box.ymin, box.zmax);
        glVertex3d(box.xmin, box.ymin, box.zmax);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex3d(box.xmin, box.ymax, box.zmin);
        glVertex3d(box.xmax, box.ymax, box.zmin);
        glVertex3d(box.xmax, box.ymax, box.zmax);
        glVertex3d(box.xmin, box.ymax, box.zmax);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex3d(box.xmin, box.ymin, box.zmin);
        glVertex3d(box.xmin, box.ymin, box.zmax);
        glVertex3d(box.xmin, box.ymax, box.zmax);
        glVertex3d(box.xmin, box.ymax, box.zmin);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex3d(box.xmax, box.ymin, box.zmin);
        glVertex3d(box.xmax, box.ymin, box.zmax);
        glVertex3d(box.xmax, box.ymax, box.zmax);
        glVertex3d(box.xmax, box.ymax, box.zmin);
        glEnd();
    }

    glColor4f(colR, colG, colB, 0.5);

    if (mode == 2 || mode == 3)
    {
        glBegin(GL_QUADS);
        glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmin - EPS);
        glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmin - EPS);
        glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmin - EPS);
        glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmin - EPS);
        glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmax + EPS);
        glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmax + EPS);
        glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmax + EPS);
        glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmax + EPS);
        glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmin - EPS);
        glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmin - EPS);
        glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmax + EPS);
        glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmax + EPS);
        glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmin - EPS);
        glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmin - EPS);
        glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmax + EPS);
        glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmax + EPS);
        glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmin - EPS);
        glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmax + EPS);
        glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmax + EPS);
        glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmin - EPS);
        glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmin - EPS);
        glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmax + EPS);
        glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmax + EPS);
        glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmin - EPS);
        glEnd();
    }
}

}
