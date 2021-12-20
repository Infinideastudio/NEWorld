#pragma once

#include "ControlContext.h"
#include "Entity.h"
#include "Universe/World/Blocks.h"

struct ItemStack {
	item item;
	uint8_t amount;
};

enum class GameMode { Survival, Creative };
class PlayerEntity: public Entity {
public:
	PlayerEntity(Double3 pos) : Entity(pos, { .6,1.6,.6 }) {}
	PlayerEntity() : PlayerEntity({ 0,0,0 }) { spawn(); }

	void setVelocity(Double3 vel) { mVelocity = vel; }

	void spawn();

	void afterMove(Double3 velocity) override;

	bool save(std::ofstream file);

	bool load(std::ifstream file);

	bool addItem(item itemName, short amount = 1);

	bool putBlock(Int3 position, Block blockName);

	void changeGameMode(GameMode gameMode);

	void update() override;

	void render() override {}

	void controlUpdate(const ControlContext& control); // called by update thread
	void renderUpdate(const ControlContext& control, bool freeze); // called by render thread

	GameMode getGameMode() const noexcept { return mGameMode; }
	void toggleCrossWall() noexcept { mCrossWall = !mCrossWall; }
	auto getInventory() noexcept { return mInventory; }
	auto getCurrentHotbarSelection() noexcept { return mIndexInHand; }
	double getHealth() const noexcept { return mHealth; }
	double getMaxHealth() const noexcept { return mMaxHealth; }
	double getHeading() const noexcept { return mHeading; }
	double getLookUpDown() const noexcept { return mLookUpDown; }
	double getCurrentJumpSpeed() const noexcept { return mCurrentJumpSpeed; }
	bool isRunning() const noexcept { return mRunning; }
	bool isOnGround() const noexcept { return mOnGround; }
	bool isNearWall() const noexcept { return mNearWall; }
	bool isFlying() const noexcept { return mFlying; }
	bool isCrossWall() const noexcept { return mCrossWall; }
	double getSpeed() const noexcept {
		// TODO: impl super-sprint
		return mRunning ? runspeed : walkspeed;
	}
private:
	void ProcessInteract(const ControlContext& control);
	void ProcessJump();
	bool ProcessNavigate(const ControlContext& control);
	void HotbarItemSelect(const ControlContext& control);
	void StartJump();

	double mLookUpDown = 90, mHeading = 0;
	double mXLookSpeed = 0, mYLookSpeed = 0;
	Double3 mVelocityInput {0};

	int mAirJumps = 0;
	double mCurrentJumpSpeed = 0;

	GameMode mGameMode = GameMode::Survival;

	double mHealth = 20, mMaxHealth = 20;
	double mHealSpeed = 0.01;
	double mDropDamage = 5.0;

	// status
	bool mOnGround = false;
	bool mRunning = false;
	bool mNearWall = false;
	bool mInWater = false;
	bool mFlying = false;
	bool mCrossWall = false;

	ItemStack mInventory[4][10];
	ubyte mIndexInHand = 0;
};
