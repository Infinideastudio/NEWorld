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

#include "Object.h"

class PlayerObject : public Object {
public:
    explicit PlayerObject(size_t worldID)
        : Object(worldID, Double3(), Double3(), Double3(1.0, 1.0, 1.0), AABB()),
          mHeight(1.6), mWidth(0.6), mSpeed(0.2) { refreshHitbox(); }

    PlayerObject(const Object& obj)
        : Object(obj), mHeight(1.6), mWidth(0.6), mSpeed(0.2) { refreshHitbox(); }

    ~PlayerObject() override = default;

    void rotate(const Double3& rotation) { mRotation += rotation; }

    void setDirection(const Double3& direction) { mRotation = direction; }

    [[nodiscard]] Double3 getDirection() const { return mRotation; }

    void setSpeed(double speed) { mSpeed = speed; }

    [[nodiscard]] double getSpeed() const { return mSpeed; }

private:
    Double3 mDirection; // Body direction, head direction is `mRotation` in class Object
    double mHeight, mWidth, mSpeed;
    Double3 mHitboxSize;

    void refreshHitbox() {
        mHitboxSize = Double3(mWidth, mHeight, mWidth);
        mHitbox = AABB(-mHitboxSize, mHitboxSize / 2);
    }
};
