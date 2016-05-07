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
    AABB expand(double expandX, double expandY, double expandZ) const;
    // Move AABB
    void move(double deltaX, double deltaY, double deltaZ);
    // Move the center of this AABB to target position
    void moveTo(double targetX, double targetY, double targetZ);

};

#endif // !AABB_H_
