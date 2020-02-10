// 
// Core: playerobject.h
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

#include "object.h"

class PlayerObject : public Object {
public:
    explicit PlayerObject(size_t worldID)
        : Object(worldID, Vec3d(), Vec3d(), Vec3d(1.0, 1.0, 1.0), AABB()),
          mHeight(1.6), mWidth(0.6), mSpeed(0.2) { refreshHitbox(); }

    PlayerObject(const Object& obj)
        : Object(obj), mHeight(1.6), mWidth(0.6), mSpeed(0.2) { refreshHitbox(); }

    ~PlayerObject() { }

    void rotate(const Vec3d& rotation) { mRotation += rotation; }

    void setDirection(const Vec3d& direction) { mRotation = direction; }

    Vec3d getDirection() const { return mRotation; }

    void setSpeed(double speed) { mSpeed = speed; }

    double getSpeed() const { return mSpeed; }

private:
    Vec3d mDirection; // Body direction, head direction is `mRotation` in class Object
    double mHeight, mWidth, mSpeed;
    Vec3d mHitboxSize;

    void refreshHitbox() {
        mHitboxSize = Vec3d(mWidth, mHeight, mWidth);
        mHitbox = AABB(-mHitboxSize, mHitboxSize / 2);
    }
};
