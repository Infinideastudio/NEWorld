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

void PlayerEntity::renderUpdate(const ControlContext& control, bool freeze) {
    if (freeze) {
        mYLookSpeed = mXLookSpeed = 0;
        return;
    }
    double heightExt = 0;
    if (isOnGround()) {
        //半蹲特效
        if (mCurrentJumpSpeed < -0.005) {
            if (mCurrentJumpSpeed <= -(Player::height - 0.5f))
                Player::heightExt = -(Player::height - 0.5f);
            else
                Player::heightExt = static_cast<float>(mCurrentJumpSpeed);
            TouchdownAnimTimer = control.Current.Time;
        }
        else {
            if (Player::heightExt <= -0.005) {
                Player::heightExt *= static_cast<float>(pow(0.8, (control.Current.Time - TouchdownAnimTimer) * 30));
                TouchdownAnimTimer = control.Current.Time;
            }
        }
    }

    auto timeDelta = control.Current.Time - control.Last.Time;
    const auto xpos = mPosition.X - Player::xd + timeDelta * 30.0 * Player::xd;
    const auto ypos = mPosition.Y + Player::height + Player::heightExt - Player::yd + timeDelta * 30.0 * Player::yd;
    const auto zpos = mPosition.Z - Player::zd + timeDelta * 30.0 * Player::zd;
    const auto plookupdown = mLookUpDown + mYLookSpeed;
    const auto pheading = mHeading + mXLookSpeed;

    //转头！你治好了我多年的颈椎病！
    mXLookSpeed -= (control.Current.MousePosition.X - control.Last.MousePosition.X) * mousemove;
    mYLookSpeed -= (control.Current.MousePosition.Y - control.Last.MousePosition.Y) * mousemove;
    if (glfwGetKey(MainWindow, GLFW_KEY_RIGHT) == 1)
        mXLookSpeed -= mousemove * 16 * timeDelta * 30.0;
    if (glfwGetKey(MainWindow, GLFW_KEY_LEFT) == 1)
        mXLookSpeed += mousemove * 16 * timeDelta * 30.0;
    if (glfwGetKey(MainWindow, GLFW_KEY_UP) == 1)
        mYLookSpeed -= mousemove * 16 * timeDelta * 30.0;
    if (glfwGetKey(MainWindow, GLFW_KEY_DOWN) == 1)
        mYLookSpeed += mousemove * 16 * timeDelta * 30.0;
}

void PlayerEntity::controlUpdate(const ControlContext& control) {
    ProcessInteract(control);
    
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

bool PlayerEntity::ProcessNavigate(const ControlContext& control) {
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

bool PlayerEntity::ProcessInteract(double lx, double ly, double lz) {
    bool blockClick{ false };//从玩家位置发射一条线段
    for (auto i = 0; i < selectPrecision * selectDistance; i++) {
        const auto lxl = lx;
        const auto lyl = ly;
        const auto lzl = lz;

        //线段延伸
        lx += sin(M_PI / 180 * (Player::heading - 180)) * sin(M_PI / 180 * (Player::lookupdown + 90)) /
            static_cast<double>(selectPrecision);
        ly += cos(M_PI / 180 * (Player::lookupdown + 90)) / static_cast<double>(selectPrecision);
        lz += cos(M_PI / 180 * (Player::heading - 180)) * sin(M_PI / 180 * (Player::lookupdown + 90)) /
            static_cast<double>(selectPrecision);

        //碰到方块
        if (BlockInfo(World::GetBlock({ RoundInt(lx), RoundInt(ly), RoundInt(lz) })).isSolid()) {
            const auto x = RoundInt(lx);
            const auto y = RoundInt(ly);
            const auto z = RoundInt(lz);
            const auto xl = RoundInt(lxl);
            const auto yl = RoundInt(lyl);
            const auto zl = RoundInt(lzl);

            mCurrentSelectedBlockPos = { x,y,z };
            mIsSelectingBlock = true;

            //找方块所在区块及位置
            mSelectedBlockBrightness = World::getbrightness(xl, yl, zl);
            mCurrentSelectedBlock = World::GetBlock({ (x), (y), (z) });
            if (mb == 1 || glfwGetKey(MainWindow, GLFW_KEY_ENTER) == GLFW_PRESS) {
                Particles::throwParticle(
                    mCurrentSelectedBlock,
                    float(x + rnd() - 0.5f), float(y + rnd() - 0.2f),
                    float(z + rnd() - 0.5f),
                    float(rnd() * 0.2f - 0.1f), float(rnd() * 0.2f - 0.1f),
                    float(rnd() * 0.2f - 0.1f),
                    float(rnd() * 0.01f + 0.02f), int(rnd() * 30) + 30
                );
                // Reset progress if selecting a different block
                if (mCurrentSelectedBlockPos != mLastSelectedBlockPos) mBlockDestructionProgress = 0.0;
                else {
                    float factor{};
                    if (Player::inventory[3][Player::indexInHand] == STICK)factor = 4;
                    else
                        factor = 30.0 /
                        (BlockInfo(Player::inventory[3][Player::indexInHand]).getHardness() + 0.1);
                    if (factor < 1.0)factor = 1.0;
                    if (factor > 1.7)factor = 1.7;
                    mBlockDestructionProgress += BlockInfo(mCurrentSelectedBlock).getHardness() *
                        ((Player::gamemode == Player::Creative) ? 10.0f : 0.3f) * factor;
                    blockClick = true;
                }

                if (mBlockDestructionProgress >= 100.0) {
                    for (auto j = 1; j <= 25; j++) {
                        Particles::throwParticle(
                            mCurrentSelectedBlock,
                            float(x + rnd() - 0.5f), float(y + rnd() - 0.2f),
                            float(z + rnd() - 0.5f),
                            float(rnd() * 0.2f - 0.1f), float(rnd() * 0.2f - 0.1f),
                            float(rnd() * 0.2f - 0.1f),
                            float(rnd() * 0.02 + 0.03), int(rnd() * 60) + 30);
                    }
                    World::PickBlock({ (x), (y), (z) });
                    blockClick = true;
                }
            }
            if (((mb == 2 && mbp == 0) || (!mChatMode && isPressed(GLFW_KEY_TAB)))) { //鼠标右键
                if (Player::inventoryAmount[3][Player::indexInHand] > 0 &&
                    isBlock(Player::inventory[3][Player::indexInHand])) {
                    //放置方块
                    if (Player::putBlock(xl, yl, zl, Player::BlockInHand)) {
                        Player::inventoryAmount[3][Player::indexInHand]--;
                        if (Player::inventoryAmount[3][Player::indexInHand] == 0)
                            Player::inventory[3][Player::indexInHand] = Blocks::ENV;

                        blockClick = true;
                    }
                }
                else {
                    //使用物品
                    if (Player::inventory[3][Player::indexInHand] == APPLE) {
                        Player::inventoryAmount[3][Player::indexInHand]--;
                        if (Player::inventoryAmount[3][Player::indexInHand] == 0)
                            Player::inventory[3][Player::indexInHand] = Blocks::ENV;
                        Player::health = Player::healthMax;
                    }
                }
            }
            break;
        }
    }

    if (mCurrentSelectedBlockPos != mLastSelectedBlockPos ||
        (mb == 0 && glfwGetKey(MainWindow, GLFW_KEY_ENTER) != GLFW_PRESS))
        mBlockDestructionProgress = 0.0;
    mLastSelectedBlockPos = mCurrentSelectedBlockPos;
    return blockClick;
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
    if (mOnGround) mVelocity *= Double3(.7, 0, .7);
}

bool PlayerEntity::putBlock(Int3 position, Block blockname) {
    auto success = false;
    if (!World::ChunkOutOfBound(getChunkPosition())
        && ((!AABB::Intersect(bounding_box(), AABB::BoxForBlock(position)) || mCrossWall ||
            !BlockInfo(blockname).isSolid()) && !BlockInfo(World::GetBlock(position)).isSolid())) {
        World::PutBlock(position, blockname);
        success = true;
    }
    return success;
}

bool PlayerEntity::save(std::ofstream isave) {
    return false;
    // isave.write((char*)&curversion, sizeof(curversion));
    // isave.write((char*)&Pos, sizeof(Pos));
    // isave.write((char*)&lookupdown, sizeof(lookupdown));
    // isave.write((char*)&heading, sizeof(heading));
    // isave.write((char*)&jump, sizeof(jump));
    // isave.write((char*)&OnGround, sizeof(OnGround));
    // isave.write((char*)&Running, sizeof(Running));
    // isave.write((char*)&AirJumps, sizeof(AirJumps));
    // isave.write((char*)&Flying, sizeof(Flying));
    // isave.write((char*)&CrossWall, sizeof(CrossWall));
    // isave.write((char*)&indexInHand, sizeof(indexInHand));
    // isave.write((char*)&health, sizeof(health));
    // isave.write((char*)&gamemode, sizeof(gamemode));
    // isave.write((char*)&gametime, sizeof(gametime));
    // isave.write((char*)inventory, sizeof(inventory));
    // isave.write((char*)inventoryAmount, sizeof(inventoryAmount));
    // isave.close();
    // return true;
}

bool PlayerEntity::load(std::ifstream file) {
    return false;
    // uint32 targetVersion;
    // std::stringstream ss;
    // ss << "Worlds/" << worldn << "/player.NEWorldPlayer";
    // std::ifstream iload(ss.str().c_str(), std::ios::binary | std::ios::in);
    // if (!iload.is_open()) return false;
    // iload.read((char*)&targetVersion, sizeof(targetVersion));
    // if (targetVersion != VERSION) return false;
    // iload.read((char*)&Pos, sizeof(Pos));
    // iload.read((char*)&lookupdown, sizeof(lookupdown));
    // iload.read((char*)&heading, sizeof(heading));
    // iload.read((char*)&jump, sizeof(jump));
    // iload.read((char*)&OnGround, sizeof(OnGround));
    // iload.read((char*)&Running, sizeof(Running));
    // iload.read((char*)&AirJumps, sizeof(AirJumps));
    // iload.read((char*)&Flying, sizeof(Flying));
    // iload.read((char*)&CrossWall, sizeof(CrossWall));
    // iload.read((char*)&indexInHand, sizeof(indexInHand));
    // iload.read((char*)&health, sizeof(health));
    // iload.read((char*)&gamemode, sizeof(gamemode));
    // iload.read((char*)&gametime, sizeof(gametime));
    // iload.read((char*)inventory, sizeof(inventory));
    // iload.read((char*)inventoryAmount, sizeof(inventoryAmount));
    // iload.close();
    // return true;
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
