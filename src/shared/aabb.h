#ifndef AABB_H_
#define AABB_H_

#include <algorithm>

struct AABB
{
    double xmin, ymin, zmin; // Min bound
    double xmax, ymax, zmax; // Max bound

    AABB() = default;

    AABB(double _xmin, double _ymin, double _zmin, double _xmax, double _ymax, double _zmax)
        :xmin(_xmin), ymin(_ymin), zmin(_zmin), xmax(_xmax), ymax(_ymax), zmax(_xmax) {}

    // Is intersect on X axis
    bool intersectX(const AABB &another_aabb) const;
    // Is intersect on Y axis
    bool intersectY(const AABB &another_aabb) const;
    // Is intersect on Z axis
    bool intersectZ(const AABB &another_aabb) const;
    // Is intersect on all axes
    bool intersect(const AABB &another_aabb) const;
    // Get max move distance <= original_move_distance on X axis, when blocked by another AABB
    double maxMoveOnXclip(const AABB &another_aabb, double original_move_distance) const;
    // Get max move distance <= original_move_distance on Y axis, when blocked by another AABB
    double maxMoveOnYclip(const AABB &another_aabb, double original_move_distance) const;
    // Get max move distance <= original_move_distance on Z axis, when blocked by another AABB
    double maxMoveOnZclip(const AABB &another_aabb, double original_move_distance) const;
    // Get expanded AABB
    AABB expand(double expand_x, double expand_y, double expand_z) const;
    // Move AABB
    void move(double delta_x, double delta_y, double delta_z);
    // Move the center of this AABB to target position
    void moveTo(double target_x, double target_y, double target_z);

};

#endif // !AABB_H_
