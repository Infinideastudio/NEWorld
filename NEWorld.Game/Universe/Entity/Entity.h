#pragma once

#include <optional>

#include "bvh.h"
#include "Math/Vector3.h"

class Entity {
public:
    Entity(Double3 position, Double3 size) : mPosition(position), mSize(size), mVelocity() {}

    [[nodiscard]] Vector3 center() const { return mPosition; }
    [[nodiscard]] BoundingBox bounding_box() const {
	    return { mPosition - mSize / 2, mPosition + mSize / 2 };
    }
    [[nodiscard]] BoundingBox movement_bounding_box() const {
        auto box = bounding_box();
        box.min += mVelocity;
        box.max += mVelocity;
        return bounding_box().extend(box);
    }

    [[nodiscard]] bool intersect(const BoundingBox& box) const {
        return AABB::Intersect(bounding_box(), box);
    }

    Double3 getPosition() const noexcept { return mPosition; }
    Double3 getSize() const noexcept { return mSize; }
    Double3 getVelocity() const noexcept { return mVelocity; }
    virtual bool doCollisionCheck() const noexcept { return true; }

    // Move the position by velocity. Will use World for collision check
    void move(const EntityBVH& bvh);

protected:
    Double3 mPosition, mSize, mVelocity;
};
