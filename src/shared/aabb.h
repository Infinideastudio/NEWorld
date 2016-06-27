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

#ifndef AABB_H_
#define AABB_H_

#include <algorithm>
#include "vec3.h"

/// Axis aligned bounding box
class AABB
{
public:
    /// Min bound
    Vec3d min;
    /// Max bound
    Vec3d max;

    AABB() = default;
    AABB(const Vec3d& _min, const Vec3d& _max) :min(_min), max(_max) {}
    AABB(const AABB&) = default;
    AABB(AABB&&) = default;

    /// Is intersect on X axis
    bool intersectX(const AABB &box) const
    {
        if ((min.x > box.min.x && min.x < box.max.x) || (max.x > box.min.x && max.x < box.max.x) ||
            (box.min.x > min.x && box.min.x < max.x) || (box.max.x > min.x && box.max.x < max.x))
            return true;

        return false;
    }
    /// Is intersect on Y axis
    bool intersectY(const AABB &box) const
    {
        if ((min.y > box.min.y && min.y < box.max.y) || (max.y > box.min.y && max.y < box.max.y) ||
            (box.min.y > min.y && box.min.y < max.y) || (box.max.y > min.y && box.max.y < max.y))
            return true;

        return false;
    }
    /// Is intersect on Z axis
    bool intersectZ(const AABB &box) const
    {
        if ((min.z > box.min.z && min.z < box.max.z) || (max.z > box.min.z && max.z < box.max.z) ||
            (box.min.z > min.z && box.min.z < max.z) || (box.max.z > min.z && box.max.z < max.z))
            return true;

        return false;
    }
    /// Is intersect on all axes
    bool intersect(const AABB &box) const
    {
        return intersectX(box) && intersectY(box) && intersectZ(box);
    }
    /// Get max move distance <= original_move_distance on X axis, when blocked by another AABB
    double maxMoveOnXclip(const AABB &box, double orgmove) const
    {
        if (!(intersectY(box) && intersectZ(box)))
            return orgmove;

        else if (min.x >= box.max.x && orgmove < 0.0)
            return std::max(box.max.x - min.x, orgmove);

        else if (max.x <= box.min.x && orgmove > 0.0)
            return std::min(box.min.x - max.x, orgmove);

        return orgmove;
    }
    /// Get max move distance <= original_move_distance on Y axis, when blocked by another AABB
    double maxMoveOnYclip(const AABB &box, double orgmove) const
    {
        if (!(intersectX(box) && intersectZ(box)))
            return orgmove;

        else if (min.y >= box.max.y && orgmove < 0.0)
            return std::max(box.max.y - min.y, orgmove);

        else if (max.y <= box.min.y && orgmove > 0.0)
            return std::min(box.min.y - max.y, orgmove);

        return orgmove;
    }
    /// Get max move distance <= original_move_distance on Z axis, when blocked by another AABB
    double maxMoveOnZclip(const AABB &box, double orgmove) const
    {
        if (!(intersectX(box) && intersectY(box)))
            return orgmove;

        else if (min.z >= box.max.z && orgmove < 0.0)
            return std::max(box.max.z - min.z, orgmove);

        else if (max.z <= box.min.z && orgmove > 0.0)
            return std::min(box.min.z - max.z, orgmove);

        return orgmove;
    }
    /// Get expanded AABB
    AABB expand(const Vec3d& arg) const
    {
        AABB res = *this;

        if (arg.x > 0.0)
            res.max.x += arg.x;
        else
            res.min.x += arg.x;

        if (arg.y > 0.0)
            res.max.y += arg.y;
        else
            res.min.y += arg.y;

        if (arg.z > 0.0)
            res.max.z += arg.z;
        else
            res.min.z += arg.z;

        return res;
    }
    /// Move AABB
    void move(const Vec3d& arg)
    {
        min += arg;
        max += arg;
    }
    /// Move the center of this AABB to target position
    void moveTo(const Vec3d& target)
    {
        double l, w, h;
        l = (max.x - min.x) / 2;
        w = (max.y - min.y) / 2;
        h = (max.z - min.z) / 2;
        min.x = target.x - l;
        min.y = target.y - w;
        min.z = target.z - h;
        max.x = target.x + l;
        max.y = target.y + w;
        max.z = target.z + h;
    }
};

#endif // !AABB_H_
