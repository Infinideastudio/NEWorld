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
	PlayerEntity(Double3 pos) : Entity(pos, { .6,1.7,.6 }) {}
	PlayerEntity() : PlayerEntity({ 0,0,0 }) { spawn(); }

	void afterMove(Double3 velocity) override;

	void render() override {}
	void update() override;
	void controlUpdate(const ControlContext& control); // called by update thread
	RenderProperties renderUpdate(const ControlContext& control, bool freeze, double lastUpdate); // called by render thread

	void spawn();
	bool addItem(item itemName, short amount = 1);
	bool placeBlock(Int3 position, Block blockName);
	void setGameMode(GameMode gameMode);
	GameMode getGameMode() const noexcept { return mGameMode; }
	void setVelocity(Double3 vel) { mVelocity = vel; }
	void toggleCrossWall() noexcept { mCrossWall = !mCrossWall; }
	auto getInventory() noexcept { return mInventory; }
	ItemStack& getCurrentSelectedItem() noexcept { return mInventory[3][mIndexInHand]; }
	int getCurrentHotbarSelection() noexcept { return mIndexInHand; }
	double getHealth() const noexcept { return mHealth; }
	void setHealth(double health) noexcept { mHealth = health; }
	double getMaxHealth() const noexcept { return mMaxHealth; }
	double getCurrentJumpSpeed() const noexcept { return mCurrentJumpSpeed; }
	bool isRunning() const noexcept { return mRunning; }
	bool isOnGround() const noexcept { return mOnGround; }
	bool isNearWall() const noexcept { return mNearWall; }
	bool isFlying() const noexcept { return mFlying; }
	bool isCrossWall() const noexcept { return mCrossWall; }
	double getSpeed() const noexcept {
		// TODO: impl super-sprint
		return (mRunning ? runspeed : walkspeed) * mSpeedBoost;
	}
	RenderProperties getPropertiesForRender(double timeDelta) const noexcept override {
		auto props = Entity::getPropertiesForRender(timeDelta);
		props.position.Y += mHeightExt + mHeight - mSize.Y / 2;
		return props;
	}

private:
	void ProcessInteract(const ControlContext& control);
	void ProcessJump();
	void ProcessNavigate(const ControlContext& control);
	void HotbarItemSelect(const ControlContext& control);
	void StartJump();

	int mAirJumps = 0;
	double mCurrentJumpSpeed = 0;

	GameMode mGameMode = GameMode::Survival;

	double mHealth = 20, mMaxHealth = 20;
	double mHealSpeed = 0.01;
	double mDropDamage = 5.0;

	double mSpeedBoost = 1;
	double mHeight = 1.7; // height of eyes
	double mHeightExt = 0;

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
