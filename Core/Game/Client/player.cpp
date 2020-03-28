// 
// Core: player.cpp
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

#include "player.h"
#include "Common/Math/Matrix.h"
#include "Common/JsonHelper.h"
#include <Game/SyncService/taskdispatcher.hpp>

class PlayerUpdateTask : public ReadOnlyTask {
public:
    PlayerUpdateTask(Player& player, size_t worldId): mPlayer(player), mWorldId(worldId) {}

    void task(const ChunkService& cs) override { mPlayer.update(*cs.getWorlds().getWorld(mWorldId)); }

    std::unique_ptr<ReadOnlyTask> clone() override { return std::make_unique<PlayerUpdateTask>(*this); }

private:
    Player& mPlayer;
    size_t mWorldId;
};

void Player::move(const World& world) {
    //mSpeed.normalize();
    //m.speed *= PlayerSpeed;
    mPositionDelta = Mat4d::rotation(mRotation.Y, Double3(0.0, 1.0, 0.0)).transform(mSpeed, 0.0).first;
    Double3 originalDelta = mPositionDelta;
    std::vector<AABB> hitboxes = world.getHitboxes(getHitbox().expand(mPositionDelta));

    for (auto& curr : hitboxes)
        mPositionDelta.X = getHitbox().maxMoveOnXclip(curr, mPositionDelta.X);
    moveHitbox(Double3(mPositionDelta.X, 0.0, 0.0));
    if (mPositionDelta.X != originalDelta.X) mSpeed.X = 0.0;

    for (auto& curr : hitboxes)
        mPositionDelta.Z = getHitbox().maxMoveOnZclip(curr, mPositionDelta.Z);
    moveHitbox(Double3(0.0, 0.0, mPositionDelta.Z));
    if (mPositionDelta.Z != originalDelta.Z) mSpeed.Z = 0.0;

    for (auto& curr : hitboxes)
        mPositionDelta.Y = getHitbox().maxMoveOnYclip(curr, mPositionDelta.Y);
    moveHitbox(Double3(0.0, mPositionDelta.Y, 0.0));
    if (mPositionDelta.Y != originalDelta.Y) mSpeed.Y = 0.0;

    mPosition += mPositionDelta;

    mOnGround = mPositionDelta.Y == 0;

    mSpeed *= 0.8;
    //mSpeed += Double3(0.0, -0.05, 0.0);
}

void Player::rotationMove() {
    static bool rotationInteria = getJsonValue<bool>(getSettings()["gui"]["rotation_interia"], false);

    if (mRotation.X + mRotationSpeed.X > 90.0)
        mRotationSpeed.X = 90.0 - mRotation.X;
    if (mRotation.X + mRotationSpeed.X < -90.0)
        mRotationSpeed.X = -90.0 - mRotation.X;
    mRotation += mRotationSpeed;
    mRotationDelta = mRotationSpeed;
    if (rotationInteria) mRotationSpeed *= 0.6;
    else mRotationSpeed = Double3(0.0);
}

Player::Player(size_t worldID) : PlayerObject(worldID) {
    // Register update event
    TaskDispatch::addRegular(
            std::make_unique<PlayerUpdateTask>(*this, mWorldID)
    );
}

void Player::render() {
    // Player model not finished yet
    /*
    glDisable(GL_CULL_FACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    // X
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(mPosition.X - 2.0f, mPosition.Y, mPosition.Z);
    glVertex3f(mPosition.X + 2.0f, mPosition.Y, mPosition.Z);
    glEnd();
    //Y
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(mPosition.X, mPosition.Y - 2.0f, mPosition.Z);
    glVertex3f(mPosition.X, mPosition.Y + 2.0f, mPosition.Z);
    glEnd();
    //Z
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(mPosition.X, mPosition.Y, mPosition.Z - 2.0f);
    glVertex3f(mPosition.X, mPosition.Y, mPosition.Z + 2.0f);
    glEnd();
    glEnable(GL_CULL_FACE);
    */
}
