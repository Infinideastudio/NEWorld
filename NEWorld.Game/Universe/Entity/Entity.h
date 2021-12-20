#pragma once

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
        return bounding_box().extend(AABB::Move(bounding_box(), mVelocity));
    }

    [[nodiscard]] bool intersect(const BoundingBox& box) const {
        return AABB::Intersect(bounding_box(), box);
    }

    [[nodiscard]] Double3 getPosition() const noexcept { return mPosition; }
    [[nodiscard]] Double3 getSize() const noexcept { return mSize; }
    [[nodiscard]] Double3 getVelocity() const noexcept { return mVelocity; }
    [[nodiscard]] virtual bool doCollisionCheck() const noexcept { return true; }
    Int3 getChunkPosition() const noexcept;

    // Move the position by velocity. Will use World for collision check
    void move(const EntityBVH& bvh);
    virtual void afterMove(Double3 actualMovement) {}

    virtual void update() = 0;
    virtual void render() = 0;

    [[nodiscard]] Double3 getVelocityForRendering() const noexcept {
        // The actual velocity used for movement in this frame (before collision checks)
        // can be used for inter-frame interpolation.
        return mVelocityForRendering;
    }
protected:
    Double3 mPosition, mSize, mVelocity;
private:
    Double3 mVelocityForRendering;
};
