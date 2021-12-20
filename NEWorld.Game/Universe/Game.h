#pragma once

#include "CommandHandler.h"
#include "ControlContext.h"
#include "Common/Logger.h"
#include "Entity/Entity.h"
#include "Entity/PlayerEntity.h"

class Game : public CommandHandler {
    Vec3<int> mLastSelectedBlockPos{};
    Brightness mSelectedBlockBrightness{};
    std::vector<std::unique_ptr<Entity>> mEntities{};
protected:
    PlayerEntity* mPlayer = nullptr;
    ControlContext mControls{ MainWindow };

    bool DebugHitbox{};
    bool DebugMergeFace{};
    bool DebugMode{};
    bool DebugShadow{};

    bool mShouldRenderGUI{};
    bool mIsSelectingBlock{};
    Block mCurrentSelectedBlock{};
    float mBlockDestructionProgress{};
    Vec3<int> mCurrentSelectedBlockPos{};

    bool mBagOpened = false;
public:
    void InitGame() {
        infostream << "Init player...";
        auto player = std::make_unique<PlayerEntity>(); // TODO: try to load first
        mPlayer = player.get();
        mEntities.emplace_back(std::move(player));
    }
    
    void updateGame() {
        //时间
        gametime++;
        if (glfwGetKey(MainWindow, GLFW_KEY_F8)) gametime += 30;
        if (gametime > gameTimeMax) gametime = 0;

        World::unloadedChunks = 0;
        World::rebuiltChunks = 0;
        World::updatedChunks = 0;

        //cpArray move
        const auto playerChunk = mPlayer->getChunkPosition();
        const auto shiftedChunkStart = playerChunk - Int3(viewdistance + 2);
        World::cpArray.MoveTo(shiftedChunkStart);

        //HeightMap move
        if (World::HMap.originX != shiftedChunkStart.X * 16 ||
            World::HMap.originZ != shiftedChunkStart.Z * 16) {
            World::HMap.moveTo(shiftedChunkStart.X * 16, shiftedChunkStart.Z * 16);
        }

        if (FirstUpdateThisFrame) {
            ChunkLoadUnload();
        }

        //加载动画
        for (auto cp : World::chunks) {
            if (cp->loadAnim <= 0.3f) cp->loadAnim = 0.0f;
            else cp->loadAnim *= 0.6f;
        }

        //随机状态更新
        RandomTick();

        mIsSelectingBlock = false;
    	mCurrentSelectedBlock = mSelectedBlockBrightness = 0;
        mCurrentSelectedBlockPos = {};

        if (!mBagOpened) {
            ProcessInteract();
            mPlayer->controlUpdate(mControls);
            HotkeySettingsToggle();
        }

        if (isPressed(GLFW_KEY_E) && mShouldRenderGUI) {
            mBagOpened = !mBagOpened;
            bagAnimTimer = timer();
            if (mBagOpened) {
                shouldGetThumbnail = true;
            }
        }
        
        Particles::updateall();
        
        EntitiesUpdate();
        EntitiesMovement();
    }

    void EntitiesUpdate() {
        // update all entities
        for (auto& entity : mEntities) {
            entity->update();
        }
    }
    void EntitiesMovement() {
        // movement of entities
        if (mEntities.empty()) return;
        EntityBVH bvh(mEntities, true);
        for (auto& entity : mEntities) {
            entity->move(bvh);
        }
    }

    void ProcessInteract() {
        auto pos = mPlayer->getPosition();
        Int3 blockBefore = pos;
        const auto heading = mPlayer->getHeading();
        const auto lookUpDown = mPlayer->getLookUpDown();
        for (auto i = 0; i < selectPrecision * selectDistance; i++) {
            //线段延伸
            pos.X += sin(M_PI / 180 * (heading - 180)) * sin(M_PI / 180 * (lookUpDown + 90)) / selectPrecision;
            pos.Y += cos(M_PI / 180 * (lookUpDown + 90)) / selectPrecision;
            pos.Z += cos(M_PI / 180 * (heading - 180)) * sin(M_PI / 180 * (lookUpDown + 90)) / selectPrecision;

            //碰到方块
            if (BlockInfo(World::GetBlock(Int3(pos, RoundInt))).isSolid()) {
                break;
            }
            blockBefore = pos;
        }

        mCurrentSelectedBlockPos = Int3(pos, RoundInt);
        mIsSelectingBlock = true;

        //找方块所在区块及位置
        mSelectedBlockBrightness = World::GetBrightness(mCurrentSelectedBlockPos);
        mCurrentSelectedBlock = World::GetBlock(mCurrentSelectedBlockPos);

        auto& itemSelection = mPlayer->getCurrentSelectedItem();

    	if (mControls.ShouldDo(ControlContext::Action::PICK_BLOCK)) {
            Particles::throwParticle(mCurrentSelectedBlock, mCurrentSelectedBlockPos);
            // Reset progress if selecting a different block
            if (mCurrentSelectedBlockPos != mLastSelectedBlockPos) mBlockDestructionProgress = 0.0;
            else {
                double factor = itemSelection.item == STICK ? 4 : 30.0 / (BlockInfo(itemSelection.item).getHardness() + 0.1);

                factor = std::clamp(factor, 1.0, 1.7);

                mBlockDestructionProgress += BlockInfo(mCurrentSelectedBlock).getHardness() *
                    (mPlayer->getGameMode() == GameMode::Creative ? 10.0f : 0.3f) * factor;
            }

            if (mBlockDestructionProgress >= 100.0) {
                for (auto j = 1; j <= 25; j++) {
                    Particles::throwParticle(mCurrentSelectedBlock, mCurrentSelectedBlockPos);
                }
                World::PickBlock(mCurrentSelectedBlockPos);
            }
        } else mBlockDestructionProgress = 0.0;

        if (mControls.ShouldDo(ControlContext::Action::PLACE_BLOCK)) { 
            if (itemSelection.amount > 0 && isBlock(itemSelection.item)) {
                //放置方块
                if (mPlayer->putBlock(blockBefore, itemSelection.item)) {
                    itemSelection.amount--;
                    if (itemSelection.amount == 0)
                        itemSelection.item = Blocks::ENV;
                }
            }
            else {
                //使用物品
                if (itemSelection.item == APPLE) {
                    itemSelection.amount--;
                    if (itemSelection.amount == 0)
                        itemSelection.item = Blocks::ENV;
                    mPlayer->setHealth(mPlayer->getMaxHealth());
                }
            }
        }
        mLastSelectedBlockPos = mCurrentSelectedBlockPos;
    }
    void ChunkLoadUnload() const {
        World::sortChunkLoadUnloadList(mPlayer->getPosition());
        for (const auto&[_, chunk] : World::ChunkUnloadList) {
            const auto c = chunk->GetPosition();
            World::DeleteChunk(c);
        }
        for (const auto&[_, pos]: World::ChunkLoadList) {
            auto c = World::AddChunk(pos);
            c->Load(false);
            if (c->Empty) {
                World::DeleteChunk(pos);
                World::cpArray.Set(pos, World::EmptyChunkPtr);
            }
        }
    }
    
    void HotkeySettingsToggle() {//各种设置切换
        if (isPressed(GLFW_KEY_F1)) {
            mPlayer->changeGameMode(mPlayer->getGameMode() == GameMode::Creative ? GameMode::Survival : GameMode::Creative);
        }
        if (isPressed(GLFW_KEY_F2)) shouldGetScreenshot = true;
        if (isPressed(GLFW_KEY_F3)) {
            DebugMode = !DebugMode;
            if (isPressed(GLFW_KEY_H)) {
                DebugHitbox = !DebugHitbox;
                DebugMode = true;
            }
            if (Renderer::AdvancedRender) {
                if (isPressed(GLFW_KEY_M)) {
                    DebugShadow = !DebugShadow;
                    DebugMode = true;
                }
            }
            else DebugShadow = false;
            if (isPressed(GLFW_KEY_G)) {
                DebugMergeFace = !DebugMergeFace;
                DebugMode = true;
            }
        }
        if (isPressed(GLFW_KEY_F4) && mPlayer->getGameMode() == GameMode::Creative)
            mPlayer->toggleCrossWall();
        if (isPressed(GLFW_KEY_F5)) mShouldRenderGUI = !mShouldRenderGUI;
        if (isPressed(GLFW_KEY_F7)) mPlayer->spawn();
        if (isPressed(GLFW_KEY_L)) World::saveAllChunks();
    }

    bool isPressed(int key) { return mControls.KeyPressed(key); }

    void RandomTick() const {
        for (auto &chunk : World::chunks) {
            const auto cPos = chunk->GetPosition();
            const auto bPos = Int3{std::min(15, int(rnd() * 16)), std::min(15, int(rnd() * 16)), std::min(15, int(rnd() * 16))};
            const auto gPos = (cPos << World::ChunkEdgeSizeLog2) + bPos;
            const auto block = chunk->GetBlock(bPos);
            if (block != Blocks::ENV) {
                BlockInfo(block).OnRandomTick(gPos, block);
            }
        }
    }
    
    static void saveGame() {
        infostream << "Saving world";
        World::saveAllChunks();
        // TODO: save player info
        //if (!Player::save(World::worldname)) {
        //    warningstream << "Failed saving player info!";
        //}
    }

    static bool loadGame() {
        // TODO: load player info
        //if (!Player::load(World::worldname)) {
        //    warningstream << "Failed loading player info!";
        //    return false;
        //}
        return true;
    }
};
