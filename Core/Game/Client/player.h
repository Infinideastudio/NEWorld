// 
// Core: player.h
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

#include <Game/SyncService/world/playerobject.h>
#include <Game/SyncService/chunkservice.hpp>

class NWCOREAPI Player : public PlayerObject {
public:
    Player(size_t worldID);

    void accelerate(const Vec3d& acceleration) { mSpeed += acceleration; }

    void accelerateRotation(const Vec3d& acceleration) { mRotationSpeed += acceleration; }

    void setSpeed(const Vec3d& speed) { mSpeed = speed; }

    double getMovingSpeed() const noexcept { return mPlayerMovingSpeed * FlyingSpeedBooster; }

    Vec3d getPositionDelta() const { return mPositionDelta; }

    Vec3d getRotationDelta() const { return mRotationDelta; }

    void render() override;

    bool isFlying() const noexcept { return mFlying; }
    void setFlying(bool fly) noexcept { mFlying = fly; }
    bool onGround() const noexcept { return mOnGround; }

    void jump() noexcept {
        if (!onGround()) return;
        mJumpProcess = mJumpHeight;
        mOnGround = false;
    }

    friend class PlayerUpdateTask;
private:
    Vec3d mSpeed, mRotationSpeed;
    Vec3d mPositionDelta, mRotationDelta;
    static constexpr double mJumpHeight = 3;
    double mJumpProcess = 0.0;
    bool mOnGround = true;
    bool mFlying = false;
    double mPlayerMovingSpeed = 0.05;
    static constexpr double FlyingSpeedBooster = 2;

    void update(const World& world) override {
        move(world);
        rotationMove();
        if(!isFlying()) accelerate(Vec3d(0.0, -0.1, 0.0)); // Gravity
        if (mJumpProcess > 0) { mJumpProcess--; accelerate(Vec3d(0.0, 0.3, 0.0)); }
    }

    void move(const World& world);
    void rotationMove();
};
