// 
// Core: AABB.h
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#pragma once

#include <algorithm>
#include <Math/Vector3.h>

// Axis aligned bounding box
class AABB {
public:
    // Min bound
    Double3 min;
    // Max bound
    Double3 max;

    AABB() = default;

    AABB(const Double3& min_, const Double3& max_) : min(min_), max(max_) { }

    AABB(const AABB& rhs) = default;

    // Is intersect on X axis
    [[nodiscard]] bool intersectX(const AABB& box) const {
        return (min.X > box.min.X && min.X < box.max.X) || (max.X > box.min.X && max.X < box.max.X) ||
            (box.min.X > min.X && box.min.X < max.X) || (box.max.X > min.X && box.max.X < max.X);
    }

    // Is intersect on Y axis
    [[nodiscard]] bool intersectY(const AABB& box) const {
        return (min.Y > box.min.Y && min.Y < box.max.Y) || (max.Y > box.min.Y && max.Y < box.max.Y) ||
            (box.min.Y > min.Y && box.min.Y < max.Y) || (box.max.Y > min.Y && box.max.Y < max.Y);
    }

    // Is intersect on Z axis
    [[nodiscard]] bool intersectZ(const AABB& box) const {
        return (min.Z > box.min.Z && min.Z < box.max.Z) || (max.Z > box.min.Z && max.Z < box.max.Z) ||
            (box.min.Z > min.Z && box.min.Z < max.Z) || (box.max.Z > min.Z && box.max.Z < max.Z);
    }

    // Is intersect on all axes
    [[nodiscard]] bool intersect(const AABB& box) const { return intersectX(box) && intersectY(box) && intersectZ(box); }

    // Get max move distance <= orgmove on X axis, when blocked by another AABB
    [[nodiscard]] double maxMoveOnXclip(const AABB& box, double orgmove) const {
        if (!(intersectY(box) && intersectZ(box)))
            return orgmove;
        if (min.X >= box.max.X && orgmove < 0.0)
            return std::max(box.max.X - min.X, orgmove);
        if (max.X <= box.min.X && orgmove > 0.0)
            return std::min(box.min.X - max.X, orgmove);

        return orgmove;
    }

    // Get max move distance <= orgmove on Y axis, when blocked by another AABB
    [[nodiscard]] double maxMoveOnYclip(const AABB& box, double orgmove) const {
        if (!(intersectX(box) && intersectZ(box)))
            return orgmove;
        if (min.Y >= box.max.Y && orgmove < 0.0)
            return std::max(box.max.Y - min.Y, orgmove);
        if (max.Y <= box.min.Y && orgmove > 0.0)
            return std::min(box.min.Y - max.Y, orgmove);

        return orgmove;
    }

    // Get max move distance <= orgmove on Z axis, when blocked by another AABB
    [[nodiscard]] double maxMoveOnZclip(const AABB& box, double orgmove) const {
        if (!(intersectX(box) && intersectY(box)))
            return orgmove;
        if (min.Z >= box.max.Z && orgmove < 0.0)
            return std::max(box.max.Z - min.Z, orgmove);
        if (max.Z <= box.min.Z && orgmove > 0.0)
            return std::min(box.min.Z - max.Z, orgmove);

        return orgmove;
    }

    // Get expanded AABB
    [[nodiscard]] AABB expand(const Double3& arg) const {
        AABB res = *this;

        if (arg.X > 0.0)
            res.max.X += arg.X;
        else
            res.min.X += arg.X;

        if (arg.Y > 0.0)
            res.max.Y += arg.Y;
        else
            res.min.Y += arg.Y;

        if (arg.Z > 0.0)
            res.max.Z += arg.Z;
        else
            res.min.Z += arg.Z;

        return res;
    }

    // Move AABB
    void move(const Double3& arg) {
        min += arg;
        max += arg;
    }
};
