// 
// Core: object.h
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

#include "Common/Physics/AABB.h"

class World;

class Object {
public:
    Object(size_t worldID) : mWorldID(worldID), mScale(1.0, 1.0, 1.0) { }

    Object(size_t worldID, const Vec3d& position, const Vec3d& rotation, const Vec3d& scale, const AABB& hitbox)
        : mWorldID(worldID), mPosition(position), mRotation(rotation), mScale(scale), mHitbox(hitbox) { }

    virtual ~Object() { }

    const Vec3d& getPosition() const { return mPosition; }

    void setPosition(const Vec3d& val) {
        mHitbox.move(val - mPosition);
        mPosition = val;
    }

    const Vec3d& getRotation() const { return mRotation; }

    void setRotation(const Vec3d& val) { mRotation = val; }

    const Vec3d& getScale() const { return mScale; }

    void setScale(const Vec3d& val) { mScale = val; }

    const AABB& getHitbox() const { return mHitbox; }

    void setHitbox(const AABB& aabb) { mHitbox = aabb; }

    void moveHitbox(const Vec3d& delta) { mHitbox.move(delta); }

    size_t getWorldID() const noexcept { return mWorldID; }

    virtual void render() = 0;
    virtual void update(const World& world) = 0;

protected:
    Vec3d mPosition, mRotation, mScale;
    AABB mHitbox;
    size_t mWorldID;
};
