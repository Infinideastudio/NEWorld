/*
* NEWorld: A free game with similar rules to Minecraft.
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

#include "aabb.h"

bool AABB::intersectX(const AABB &box) const
{
    if ((min.x > box.min.x && min.x < box.max.x) || (max.x > box.min.x && max.x < box.max.x) ||
            (box.min.x > min.x && box.min.x < max.x) || (box.max.x > min.x && box.max.x < max.x))
        return true;

    return false;
}

bool AABB::intersectY(const AABB &box) const
{
    if ((min.y > box.min.y && min.y < box.max.y) || (max.y > box.min.y && max.y < box.max.y) ||
            (box.min.y > min.y && box.min.y < max.y) || (box.max.y > min.y && box.max.y < max.y))
        return true;

    return false;
}

bool AABB::intersectZ(const AABB &box) const
{
    if ((min.z > box.min.z && min.z < box.max.z) || (max.z > box.min.z && max.z < box.max.z) ||
            (box.min.z > min.z && box.min.z < max.z) || (box.max.z > min.z && box.max.z < max.z))
        return true;

    return false;
}

bool AABB::intersect(const AABB &box) const
{
    return intersectX(box) && intersectY(box) && intersectZ(box);
}

double AABB::maxMoveOnXclip(const AABB &box, double orgmove) const
{
    if (!(intersectY(box) && intersectZ(box)))
        return orgmove;

    else if (min.x >= box.max.x && orgmove < 0.0)
        return std::max(box.max.x - min.x, orgmove);

    else if (max.x <= box.min.x && orgmove > 0.0)
        return std::min(box.min.x - max.x, orgmove);

    return orgmove;
}

double AABB::maxMoveOnYclip(const AABB &box, double orgmove) const
{
    if (!(intersectX(box) && intersectZ(box)))
        return orgmove;

    else if (min.y >= box.max.y && orgmove < 0.0)
        return std::max(box.max.y - min.y, orgmove);

    else if (max.y <= box.min.y && orgmove > 0.0)
        return std::min(box.min.y - max.y, orgmove);

    return orgmove;
}

double AABB::maxMoveOnZclip(const AABB &box, double orgmove) const
{
    if (!(intersectX(box) && intersectY(box)))
        return orgmove;

    else if (min.z >= box.max.z && orgmove < 0.0)
        return std::max(box.max.z - min.z, orgmove);

    else if (max.z <= box.min.z && orgmove > 0.0)
        return std::min(box.min.z - max.z, orgmove);

    return orgmove;
}

AABB AABB::expand(double x, double y, double z) const
{
    AABB res = *this;

    if (x > 0.0)
        res.max.x += x;
    else
        res.min.x += x;

    if (y > 0.0)
        res.max.y += y;
    else
        res.min.y += y;

    if (z > 0.0)
        res.max.z += z;
    else
        res.min.z += z;

    return res;
}

void AABB::move(double x, double y, double z)
{
    min.x += x;
    min.y += y;
    min.z += z;
    max.x += x;
    max.y += y;
    max.z += z;
}

void AABB::moveTo(double x, double y, double z)
{
    double l, w, h;
    l = (max.x - min.x) / 2;
    w = (max.y - min.y) / 2;
    h = (max.z - min.z) / 2;
    min.x = x - l;
    min.y = y - w;
    min.z = z - h;
    max.x = x + l;
    max.y = y + w;
    max.z = z + h;
}
