#include "PlayerEntity.h"
#include <fstream>
#include <sstream>

#include "ControlContext.h"
#include "Universe/World/Blocks.h"
#include "Universe/World/World.h"

void PlayerEntity::update() {
    // Process player's health
    if (mGameMode != GameMode::Creative) {
        if (mHealth > 0) {
            if (mPosition.Y < -100) mHealth -= (-100 - mPosition.Y) / 100;
            mHealth = std::min(mHealth + mHealSpeed, mMaxHealth);
        }
        else {
            spawn();
        }
    }

    if (!mFlying && !mCrossWall && World::inWater(bounding_box())) {
        mVelocity *= 0.6;
    }
}

RenderProperties PlayerEntity::renderUpdate(const ControlContext& control, bool freeze, double lastUpdate) {
    if (freeze) {
        mYLookSpeed = mXLookSpeed = 0;
        return getPropertiesForRender(0);
    }
    if (isOnGround()) {
        //半蹲特效
        if (mCurrentJumpSpeed < -0.005) {
            if (mCurrentJumpSpeed <= -(mHeight - 0.5f))
                mHeightExt = -(mHeight - 0.5f);
            else
                mHeightExt = static_cast<float>(mCurrentJumpSpeed);
            TouchdownAnimTimer = control.Current.Time;
        }
        else {
            if (mHeightExt <= -0.005) {
                mHeightExt *= static_cast<float>(pow(0.8, (control.Current.Time - TouchdownAnimTimer) * 30));
                TouchdownAnimTimer = control.Current.Time;
            }
        }
    }

    const auto timeDelta = control.Current.Time - lastUpdate;

    //转头！你治好了我多年的颈椎病！
    mXLookSpeed -= (control.Current.MousePosition.X - control.Last.MousePosition.X) * mousemove;
    mYLookSpeed += (control.Current.MousePosition.Y - control.Last.MousePosition.Y) * mousemove;
    if (control.KeyPressed(GLFW_KEY_RIGHT))
        mXLookSpeed -= mousemove * 16 * timeDelta * 30.0;
    if (control.KeyPressed(GLFW_KEY_LEFT))
        mXLookSpeed += mousemove * 16 * timeDelta * 30.0;
    if (control.KeyPressed(GLFW_KEY_UP))
        mYLookSpeed -= mousemove * 16 * timeDelta * 30.0;
    if (control.KeyPressed(GLFW_KEY_DOWN))
        mYLookSpeed += mousemove * 16 * timeDelta * 30.0;
    
    return getPropertiesForRender(timeDelta);
}

void PlayerEntity::controlUpdate(const ControlContext& control) {
    //更新方向
    mHeading = fmod(mHeading + mXLookSpeed, 360.0);
    mLookUpDown = std::clamp(mLookUpDown + mYLookSpeed, -90.0, 90.0);
    mXLookSpeed = mYLookSpeed = 0.0;

    ProcessNavigate(control);
    HotbarItemSelect(control);

    if (control.KeyPressed(GLFW_KEY_SPACE)) StartJump();

    if (control.KeyPressed(GLFW_KEY_LEFT_SHIFT) || control.KeyPressed(GLFW_KEY_RIGHT_SHIFT)) {
        if (mCrossWall || mFlying) mVelocity.Y -= walkspeed / 2;
    }

	//跳跃
    ProcessJump();
}

void PlayerEntity::ProcessNavigate(const ControlContext& control) {
    auto speed = getSpeed();
    // TODO: mRunning = true; impl running
    if (control.KeyPressed(GLFW_KEY_W)) {
        mVelocity.X += -sin(mHeading * M_PI / 180.0) * speed;
        mVelocity.Z += -cos(mHeading * M_PI / 180.0) * speed;
    }
    else {
        mRunning = false;
    }

    if (control.KeyPressed(GLFW_KEY_S) == GLFW_PRESS) {
        mVelocity.X += sin(mHeading * M_PI / 180.0) * speed;
        mVelocity.Z += cos(mHeading * M_PI / 180.0) * speed;
    }

    if (control.KeyPressed(GLFW_KEY_A) == GLFW_PRESS) {
        mVelocity.X += sin((mHeading - 90) * M_PI / 180.0) * speed;
        mVelocity.Z += cos((mHeading - 90) * M_PI / 180.0) * speed;
    }

    if (control.KeyPressed(GLFW_KEY_D) == GLFW_PRESS) {
        mVelocity.X += -sin((mHeading - 90) * M_PI / 180.0) * speed;
        mVelocity.Z += -cos((mHeading - 90) * M_PI / 180.0) * speed;
    }

    if (!mFlying && !mCrossWall) {
        const auto horizontalSpeed = sqrt(mVelocity.X * mVelocity.X + mVelocity.Z * mVelocity.Z);
        if (horizontalSpeed > speed) {
            mVelocity.X *= speed / horizontalSpeed;
            mVelocity.Z *= speed / horizontalSpeed;
        }
    }
}

void PlayerEntity::ProcessJump() {
    if (!mInWater) {
        if (!mFlying && !mCrossWall) {
            mVelocity.Y = -0.001;
            if (mOnGround) {
                mCurrentJumpSpeed = 0.0;
                mAirJumps = 0;
            }
            else {
                //自由落体计算
                mCurrentJumpSpeed -= 0.025;
                mVelocity.Y = mCurrentJumpSpeed + 0.5 * 0.6 / 900.0;
            }
        }
        else {
            mCurrentJumpSpeed = 0.0;
            mAirJumps = 0;
        }
    }
    else {
        mCurrentJumpSpeed = 0.0;
        mAirJumps = MaxAirJumps;
        if (mVelocity.Y <= 0.001 && !mFlying && !mCrossWall) {
            mVelocity.Y = -0.001;
            if (!mOnGround) mVelocity.Y -= 0.1;
        }
    }
}

void PlayerEntity::HotbarItemSelect(const ControlContext& control) {
    //切换方块
    if (control.KeyPressed(GLFW_KEY_Z) && mIndexInHand > 0) mIndexInHand--;
    if (control.KeyPressed(GLFW_KEY_X) && mIndexInHand < 9) mIndexInHand++;
    auto deltaScroll = control.Last.MouseScroll - control.Current.MouseScroll;
    if (static_cast<int>(mIndexInHand) + deltaScroll < 0)mIndexInHand = 9;
    else if (static_cast<int>(mIndexInHand) + deltaScroll > 9)mIndexInHand = 0;
    else mIndexInHand += static_cast<char>(deltaScroll);
}

void PlayerEntity::StartJump() {
    if (!mInWater) {
        if ((mOnGround || mAirJumps < MaxAirJumps) && !mFlying &&
            !mCrossWall) {
            if (!mOnGround) {
                mCurrentJumpSpeed = 0.3;
                mAirJumps++;
            }
            else {
                mCurrentJumpSpeed = 0.25;
                mOnGround = false;
            }
        }
        if (mFlying || mCrossWall) {
            mVelocity.Y += walkspeed / 2;
        }
    }
    else {
        mVelocity.Y = walkspeed;
    }
}

void PlayerEntity::spawn() {
    mPosition = { 0,60,0 };
    mVelocity = { 0,0,0 };
    mHealth = mMaxHealth;
    mCurrentJumpSpeed = 0.0;
    memset(mInventory, 0, sizeof(mInventory));
    
    for (size_t i = 0; i < 255; i++) {
        addItem(Blocks::ROCK);
        addItem(Blocks::GRASS);
        addItem(Blocks::DIRT);
        addItem(Blocks::STONE);
        addItem(Blocks::PLANK);
        addItem(Blocks::WOOD);
        addItem(Blocks::LEAF);
        addItem(Blocks::GLASS);
        addItem(Blocks::WATER);
        addItem(Blocks::LAVA);
        addItem(Blocks::GLOWSTONE);
        addItem(Blocks::SAND);
        addItem(Blocks::CEMENT);
        addItem(Blocks::ICE);
        addItem(Blocks::COAL);
        addItem(Blocks::IRON);
        addItem(Blocks::TNT);
    }
}

void PlayerEntity::afterMove(Double3 actualMovement) {
	// if we tried to go down but did not actually go down - we hit the ground/floor.
    if (actualMovement.Y != mVelocity.Y && mVelocity.Y < 0) {
        mOnGround = true;
        // fall damage
        if (mVelocity.Y < -0.4 && mGameMode == GameMode::Survival) {
            mHealth += mVelocity.Y * mDropDamage; // mVelocity.Y is negative
        }
    }
    else mOnGround = false;

    // when we hit the ceil
    if (actualMovement.Y != mVelocity.Y && mVelocity.Y > 0) mCurrentJumpSpeed = 0.0;

    // when we hit walls
    mNearWall = actualMovement.X != mVelocity.X || actualMovement.Z != mVelocity.Z;

    mVelocity = Double3(Int3(mVelocity * 100000)) / 100000.0;

    mVelocity *= 0.8;
    if (mFlying || mCrossWall) mVelocity.Y *= 0.8;
    if (mOnGround) {
        mVelocity *= .7;
        mVelocity.Y = 0;
    }
}

bool PlayerEntity::putBlock(Int3 position, Block blockname) {
    if (!World::ChunkOutOfBound(getChunkPosition())
        && (!AABB::Intersect(bounding_box(), AABB::BoxForBlock(position)) || mCrossWall || !BlockInfo(blockname).isSolid())
        && !BlockInfo(World::GetBlock(position)).isSolid()) {

        World::PutBlock(position, blockname);
        return true;

    }
    return false;
}

bool PlayerEntity::addItem(item itemname, short amount) {
    const auto InvMaxStack = 255;
    for (auto i = 3; i >= 0; i--) {
        for (auto j = 0; j != 10; j++) {
            if (mInventory[i][j].item == itemname && mInventory[i][j].amount < InvMaxStack) {
                if (amount + mInventory[i][j].amount <= InvMaxStack) {
                    mInventory[i][j].amount += amount;
                    return true;
                }
                amount -= InvMaxStack - mInventory[i][j].amount;
                mInventory[i][j].amount = InvMaxStack;
            }
        }
    }
    for (auto i = 3; i >= 0; i--) {
        for (auto j = 0; j != 10; j++) {
            if (mInventory[i][j].item == Blocks::ENV) {
                mInventory[i][j].item = itemname;
                if (amount <= InvMaxStack) {
                    mInventory[i][j].amount = amount;
                    return true;
                }
                mInventory[i][j].amount = InvMaxStack;
                amount -= InvMaxStack;
            }
        }
    }
    return false;
}

void PlayerEntity::changeGameMode(GameMode _gamemode) {
    mGameMode = _gamemode;
    switch (_gamemode) {
    case GameMode::Survival:
        mFlying = false;
        mCrossWall = false;
        mCurrentJumpSpeed = 0.0;
        break;

    case GameMode::Creative:
        mFlying = true;
        mCurrentJumpSpeed = 0.0;
        break;
    }
}
