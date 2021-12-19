#include "bvh.h"

#include <bvh/sweep_sah_builder.hpp>
#include <bvh/locally_ordered_clustering_builder.hpp>
#include <bvh/primitive_intersectors.hpp>
#include "Entity.h"

template <typename Bvh, typename Primitive, bool Permuted = false>
struct AABBIntersector : public bvh::PrimitiveIntersector<Bvh, Primitive, Permuted, false> {
    AABBIntersector(const Bvh& bvh, const Primitive* primitives)
        : bvh::PrimitiveIntersector<Bvh, Primitive, Permuted, false>(bvh, primitives) {}

    std::vector<size_t> result_indices;

    bool intersect(size_t index, const BoundingBox& box, bool is_leaf) {
        auto [p, i] = this->primitive_at(index);
        if (auto hit = p->intersect(box)) {
            if (is_leaf) result_indices.push_back(i);
            return true;
        }
        return false;
    }
};

static void aabb_intersect(const Bvh& bvh, size_t index, AABBIntersector<Bvh, Entity*, true>& intersector, const BoundingBox& box) {
    const auto& node = bvh.nodes[index];
    if (node.is_leaf()) {
        for (int i = 0; i < node.primitive_count; ++i) {
            intersector.intersect(node.first_child_or_primitive + i, box, true);
        }
        return;
    }
    if (intersector.intersect(index, box, false)) {
        auto left = bvh.nodes[index].first_child_or_primitive;
        aabb_intersect(bvh, left, intersector, box);
        aabb_intersect(bvh, left + 1, intersector, box);
    }
}

EntityBVH::EntityBVH(const std::vector<std::unique_ptr<Entity>>& entities, bool for_movement) {
    std::vector<BoundingBox> bboxes;
    std::vector<Vector3> centers;

    for (const auto& e : entities) {
        bboxes.emplace_back(for_movement ? e->movement_bounding_box() : e->bounding_box());
        centers.emplace_back(e->center());
    }

    construct_bvh(bboxes, centers);
}

std::vector<Entity*> EntityBVH::intersect(const BoundingBox& box) const {
    AABBIntersector<Bvh, Entity*, true> intersector(mBvh, mEntities.data());
    aabb_intersect(mBvh, 0, intersector, box);
    std::vector<Entity*> entities;
    for (const auto index : intersector.result_indices) entities.push_back(entities[index]);
    return entities;
}

void EntityBVH::construct_bvh(const std::vector<BoundingBox>& bboxes, const std::vector<Vector3>& centers) {
    const auto global_bbox =
        bvh::compute_bounding_boxes_union(bboxes.data(), bboxes.size());

    bvh::SweepSahBuilder builder(mBvh);
    builder.build(global_bbox, bboxes.data(), centers.data(), bboxes.size());
}
