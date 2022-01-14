#pragma once

#include <bvh/bvh.hpp>
#include <bvh/vector.hpp>
#include <bvh/ray.hpp>

#include "Math/Vector3.h"

using Scalar = double;
using Vector3 = bvh::Vector3<Scalar>;
using BoundingBox = bvh::BoundingBox<Scalar>;
using Ray = bvh::Ray<Scalar>;
using Bvh = bvh::Bvh<Scalar>;

class Entity;

inline Vector3 toBvhVec(const Double3& v) { return {v.X, v.Y, v.Z}; }

class EntityBVH {
public:
    EntityBVH(const std::vector<std::unique_ptr<Entity>>& entities, bool for_movement = false);
    std::vector<Entity*> intersect(const BoundingBox& box) const;

private:
    void construct_bvh(const std::vector<BoundingBox>& bboxes, const std::vector<Vector3>& centers);

    Bvh mBvh;
    std::vector<Entity*> mEntities;
};

namespace AABB {
    inline bool InClip(const BoundingBox& boxA, const BoundingBox& boxB, int axis) {
        if (boxA.min.values[axis] > boxB.min.values[axis] && boxA.min.values[axis] < boxB.max.values[axis] ||
            boxA.max.values[axis] > boxB.min.values[axis] && boxA.max.values[axis] < boxB.max.values[axis])
            return true;
        if (boxB.min.values[axis] > boxA.min.values[axis] && boxB.min.values[axis] < boxA.max.values[axis] ||
            boxB.max.values[axis] > boxA.min.values[axis] && boxB.max.values[axis] < boxA.max.values[axis])
            return true;
        return false;
    }

    inline bool Intersect(const BoundingBox& boxA, const BoundingBox& boxB) {
        return InClip(boxA, boxB, 0) && InClip(boxA, boxB, 1) && InClip(boxA, boxB, 2);
    }

    inline double MaxMove(const BoundingBox& boxA, const BoundingBox& boxB, double distance, int axis) {
        // perform collision from A to B
        if (!((axis == 0 || InClip(boxA, boxB, 0)) &&
            (axis == 1 || InClip(boxA, boxB, 1)) &&
            (axis == 2 || InClip(boxA, boxB, 2))))
            return distance;
        if (boxA.min.values[axis] >= boxB.max.values[axis] && distance < 0.0)
            return std::max(boxB.max.values[axis] - boxA.min.values[axis], distance);
        if (boxA.max.values[axis] <= boxB.min.values[axis] && distance > 0.0)
            return std::min(boxB.min.values[axis] - boxA.max.values[axis], distance);
        return distance;
    }

    inline BoundingBox Move(BoundingBox box, Double3 direction) {
        box.min += toBvhVec(direction);
        box.max += toBvhVec(direction);
        return box;
    }

    inline BoundingBox BoxForBlock(Int3 pos) {
        return { {pos.X - 0.5, pos.Y - 0.5, pos.Z - 0.5},
        	{ pos.X + 0.5, pos.Y + 0.5, pos.Z + 0.5 } };
    }
}
