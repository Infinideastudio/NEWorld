#include "Items.h"
#include "Player.h"
#include "Blocks.h"
#include "World.h"

namespace Blocks {
    BlockType::~BlockType() noexcept = default;

    bool BlockType::BeforeBlockPlace(const Int3& position, Block block) noexcept { return true; }
    void BlockType::AfterBlockPlace(const Int3& position, Block block) noexcept { }
    bool BlockType::BeforeBlockDestroy(const Int3& position, Block block) noexcept { return true; }

    void BlockType::AfterBlockDestroy(const Int3& position, Block block) noexcept {
        // should be an item drop here
        // TODO: replace when entity is implemented
        //Player::addItem(block);
    }

    void BlockType::OnRandomTick(const Int3& position, Block block) noexcept { }

    BlockType blockData[BLOCK_DEF_END + 1] = {
        //			方块名称		  固体	 不透明	  半透明  可以爆炸  硬度
        BlockType("NEWorld.Blocks.Air", false, false, false, 0),
        BlockType("NEWorld.Blocks.Rock", true, true, false, 2),
        BlockType("NEWorld.Blocks.Grass", true, true, false, 5),
        BlockType("NEWorld.Blocks.Dirt", true, true, false, 5),
        BlockType("NEWorld.Blocks.Stone", true, true, false, 2),
        BlockType("NEWorld.Blocks.Plank", true, true, false, 5),
        BlockType("NEWorld.Blocks.Wood", true, true, false, 5),
        BlockType("NEWorld.Blocks.Bedrock", true, true, false, 0),
        BlockType("NEWorld.Blocks.Leaf", true, false, false, 15),
        BlockType("NEWorld.Blocks.Glass", true, false, false, 30),
        BlockType("NEWorld.Blocks.Water", false, false, true, 0),
        BlockType("NEWorld.Blocks.Lava", false, false, true, 0),
        BlockType("NEWorld.Blocks.GlowStone", true, true, false, 10),
        BlockType("NEWorld.Blocks.Sand", true, true, false, 8),
        BlockType("NEWorld.Blocks.Cement", true, true, false, 0.5f),
        BlockType("NEWorld.Blocks.Ice", true, false, true, 25),
        BlockType("NEWorld.Blocks.Coal Block", true, true, false, 1),
        BlockType("NEWorld.Blocks.Iron Block", true, true, false, 0.5f),
        BlockType("NEWorld.Blocks.TNT", true, true, false, 25),
        BlockType("NEWorld.Blocks.Null Block", true, true, false, 0)
    };

    class Grass : public BlockType {
    public:
        Grass()
            : BlockType(blockData[2]) { }

        void OnRandomTick(const Int3& position, Block) noexcept override {
            const auto top = position + Int3{0, 1, 0};
            //草被覆盖
            if (World::GetBlock(top) != Blocks::ENV) {
                World::SetBlock(position, Blocks::DIRT);
                World::updateblock(top.X, top.Y, top.Z, true);
            }
        }
    } gGrass;

    class Dirt : public BlockType {
    public:
        Dirt()
            : BlockType(blockData[3]) { }

        void OnRandomTick(const Int3& position, Block) noexcept override {
            const auto top = position + Int3{0, 1, 0};
            static constexpr Int3 direction[] = {
                {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1},
                {1, 1, 0}, {-1, 1, 0}, {0, 1, 1}, {0, 1, -1},
                {1, -1, 0}, {-1, -1, 0}, {0, -1, 1}, {0, -1, -1}
            };
            if (World::GetBlock(top, Blocks::NONEMPTY) == Blocks::ENV) {
                for (const auto& v : direction) {
                    if (World::GetBlock(position + v) == Blocks::GRASS) {
                        //长草
                        World::SetBlock(position, Blocks::GRASS);
                        World::updateblock(top.X, top.Y, top.Z, true);
                        return;
                    }
                }
            }
        }
    } gDirt;

    class Leaf : public BlockType {
    public:
        Leaf()
            : BlockType(blockData[8]) { }

        void AfterBlockDestroy(const Int3& position, Block) noexcept override {
            //if (rnd() < 0.2) {
            //    if (rnd() < 0.5)Player::addItem(APPLE);
            //    else Player::addItem(STICK);
            //}
            //else { Player::addItem(Blocks::LEAF); }
        }
    } gLeaf;

    class Tnt : public BlockType {
    public:
        Tnt()
            : BlockType(blockData[18]) { }

        void AfterBlockPlace(const Int3& position, Block) noexcept override {
            World::explode(position.X, position.Y, position.Z, 8, World::GetChunk(World::GetChunkPos(position)));
        }
    } gTnt;

    BlockType* TypeIndex[BLOCK_DEF_END + 1] = {
        &blockData[0], &blockData[1], &gGrass, &gDirt, &blockData[4], &blockData[5],
        &blockData[6], &blockData[7], &gLeaf, &blockData[9], &blockData[10], &blockData[11],
        &blockData[12], &blockData[13], &blockData[14], &blockData[15], &blockData[16], &blockData[17],
        &gTnt, &blockData[19]
    };
}

Blocks::BlockType& BlockInfo(int id) noexcept {
    return *Blocks::TypeIndex[id >= Blocks::BLOCK_DEF_END || id < 0 ? Blocks::BLOCK_DEF_END : id];
}
