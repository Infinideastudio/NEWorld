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

// Axis aligned bounding box
class AABB
{
public:
    Vec3d min; // Min bound
    Vec3d max; // Max bound

    AABB() {}
    AABB(const Vec3d& _min, const Vec3d& _max) :min(_min), max(_max) {}
    AABB(const AABB&) = default;
    AABB(AABB&&) = default;

    // Is intersect on X axis
    bool intersectX(const AABB &anotherAABB) const;
    // Is intersect on Y axis
    bool intersectY(const AABB &anotherAABB) const;
    // Is intersect on Z axis
    bool intersectZ(const AABB &anotherAABB) const;
    // Is intersect on all axes
    bool intersect(const AABB &anotherAABB) const;
    // Get max move distance <= original_move_distance on X axis, when blocked by another AABB
    double maxMoveOnXclip(const AABB &anotherAABB, double originalMoveDistance) const;
    // Get max move distance <= original_move_distance on Y axis, when blocked by another AABB
    double maxMoveOnYclip(const AABB &anotherAABB, double originalMoveDistance) const;
    // Get max move distance <= original_move_distance on Z axis, when blocked by another AABB
    double maxMoveOnZclip(const AABB &anotherAABB, double originalMoveDistance) const;
    // Get expanded AABB
    AABB&& expand(const Vec3d& arg) const;
    // Move AABB
    void move(const Vec3d& arg);
    // Move the center of this AABB to target position
    void moveTo(double targetX, double targetY, double targetZ);

};

#endif // !AABB_H_
