#pragma once

#include "CommandHandler.h"
#include "ControlContext.h"
#include "Common/Logger.h"
#include "Entity/Entity.h"
#include "Entity/PlayerEntity.h"

class Game : public CommandHandler {
    Vec3<int> mLastSelectedBlockPos{};
protected:
    std::vector<std::unique_ptr<Entity>> mEntities{};
    PlayerEntity* mPlayer = nullptr;
    ControlContext mControlsForUpdate{ MainWindow };

    bool DebugHitbox{};
    bool DebugMode{};
    bool DebugShadow{};

    bool mShouldRenderGUI{};
    float mBlockDestructionProgress{};
    std::optional<std::pair<Block, Vec3<int>>> mCurrentSelection{};

    bool mBagOpened = false;
public:
    void InitGame() {
        infostream << "Init player...";
        auto player = std::make_unique<PlayerEntity>(); // TODO: try to load first
        mPlayer = player.get();
        mEntities.emplace_back(std::move(player));
    }
    
    void updateGame() {
        mControlsForUpdate.Update();
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

        if (!mBagOpened) {
            // Get camera position. Used delta=0 so there might be a little errors but shouldn't be noticeable.
            const auto props = mPlayer->getPropertiesForRender(0);
            ProcessInteract(props.position, props.heading, props.lookUpDown);
            mPlayer->controlUpdate(mControlsForUpdate);
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

    // handle ray/bounding box collision check to pick/place blocks
    void ProcessInteract(Double3 startPosition, double heading, double lookUpDown) {
        auto pos = startPosition;
        Int3 blockBefore = pos;
        mCurrentSelection = std::nullopt;

        for (auto i = 0; i < selectPrecision * selectDistance; i++) {
            //线段延伸
            pos.X += sin(M_PI / 180 * (heading - 180)) * sin(M_PI / 180 * (lookUpDown + 90)) / selectPrecision;
            pos.Y += cos(M_PI / 180 * (lookUpDown + 90)) / selectPrecision;
            pos.Z += cos(M_PI / 180 * (heading - 180)) * sin(M_PI / 180 * (lookUpDown + 90)) / selectPrecision;

            const auto currentBlockPos = Int3(pos, RoundInt);
            const auto currentBlock = World::GetBlock(currentBlockPos);
            //碰到方块
            if (BlockInfo(currentBlock).isSolid()) {
                mCurrentSelection = std::make_pair(currentBlock, currentBlockPos);
                break;
            }
            blockBefore = currentBlockPos;
        }
        if (!mCurrentSelection) return;
        
        auto& itemSelection = mPlayer->getCurrentSelectedItem();

    	if (mControlsForUpdate.ShouldDo(ControlContext::Action::PICK_BLOCK)) {
            Particles::throwParticle(mCurrentSelection->first, mCurrentSelection->second);
            // Reset progress if selecting a different block
            if (mCurrentSelection->second != mLastSelectedBlockPos) mBlockDestructionProgress = 0.0;
            else {
                double factor = itemSelection.item == STICK ? 4 : 30.0 / (BlockInfo(itemSelection.item).getHardness() + 0.1);

                factor = std::clamp(factor, 1.0, 1.7);

                mBlockDestructionProgress += BlockInfo(mCurrentSelection->first).getHardness() *
                    (mPlayer->getGameMode() == GameMode::Creative ? 10.0f : 0.3f) * factor;
            }

            if (mBlockDestructionProgress >= 100.0) {
                for (auto j = 1; j <= 25; j++) {
                    Particles::throwParticle(mCurrentSelection->first, mCurrentSelection->second);
                }
                World::PickBlock(mCurrentSelection->second);
            }
        } else mBlockDestructionProgress = 0.0;

        if (mControlsForUpdate.ShouldDo(ControlContext::Action::PLACE_BLOCK)) {
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
        mLastSelectedBlockPos = mCurrentSelection->second;
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
        }
        if (isPressed(GLFW_KEY_F4) && mPlayer->getGameMode() == GameMode::Creative)
            mPlayer->toggleCrossWall();
        if (isPressed(GLFW_KEY_F5)) mShouldRenderGUI = !mShouldRenderGUI;
        if (isPressed(GLFW_KEY_F7)) mPlayer->spawn();
        if (isPressed(GLFW_KEY_L)) World::saveAllChunks();
    }

    bool isPressed(int key) { return mControlsForUpdate.KeyJustPressed(key); }

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
