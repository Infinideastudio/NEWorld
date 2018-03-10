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

#ifndef PLAYER_H_
#define PLAYER_H_

#include <sync_service/world/playerobject.h>
#include <sync_service/chunkservice.hpp>

class NWCOREAPI Player : public PlayerObject {
public:
    Player(size_t worldID);

    void accelerate(const Vec3d& acceleration) { mSpeed += acceleration; }

    void accelerateRotation(const Vec3d& acceleration) { mRotationSpeed += acceleration; }

    void setSpeed(const Vec3d& speed) { mSpeed = speed; }

    Vec3d getPositionDelta() const { return mPositionDelta; }

    Vec3d getRotationDelta() const { return mRotationDelta; }

    void render() override;

    friend class PlayerUpdateTask;
private:
    Vec3d mSpeed, mRotationSpeed;
    Vec3d mPositionDelta, mRotationDelta;

    void update(const World& world) override {
        move(world);
        rotationMove();
        accelerate(Vec3d(0.0, -0.1, 0.0)); // Gravity
    }

    void move(const World& world);
    void rotationMove();
};

#endif // !PLAYER_H_
