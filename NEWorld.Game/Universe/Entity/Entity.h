#pragma once

#include "bvh.h"
#include "Definitions.h"
#include "Math/Vector3.h"

struct RenderProperties {
    Double3 position;
    double heading, lookUpDown;
};

class Entity {
public:
    Entity(Double3 position, Double3 size) : mPosition(position), mSize(size), mVelocity() {}
    virtual ~Entity() = default;

    [[nodiscard]] Vector3 center() const { return toBvhVec(mPosition); }
    [[nodiscard]] BoundingBox bounding_box() const {
	    return { toBvhVec(mPosition - mSize / 2), toBvhVec(mPosition + mSize / 2) };
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
    
    [[nodiscard]] double getHeading() const noexcept { return mHeading; }
    [[nodiscard]] double getLookUpDown() const noexcept { return mLookUpDown; }

    [[nodiscard]] virtual RenderProperties getPropertiesForRender(double timeDelta) const noexcept {
        // timeDelta is the time since the last update frame.
        auto cameraPosition = mPosition + (timeDelta * MaxUpdateFPS - 1) * mVelocityForRendering;
        return{ cameraPosition, mHeading + mXLookSpeed, std::clamp(mLookUpDown + mYLookSpeed, -90.0, 90.0) };
    }

protected:
    Double3 mPosition, mSize, mVelocity;

    double mLookUpDown = 90, mHeading = 0;
    double mXLookSpeed = 0, mYLookSpeed = 0;
private:
    // The actual velocity used for movement in this frame
    // can be used for inter-frame interpolation.
    Double3 mVelocityForRendering;
};
