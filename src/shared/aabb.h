#ifndef AABB_H_
#define AABB_H_

#include <algorithm>

// Axis aligned bounding box
class AABB
{
    public:
        double xmin, ymin, zmin; // Min bound
        double xmax, ymax, zmax; // Max bound

        AABB();
        AABB(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax);

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
