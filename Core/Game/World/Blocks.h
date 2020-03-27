// 
// Core: nwblock.h
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

#include <vector>
#include <string>
#include <cstdint>
#include "Common/Config.h"

namespace Game::World {
    class BlockData {
    public:
        BlockData() : mBlockDataUnion{{0}} {}

        BlockData(uint32_t id, uint32_t brightness, uint32_t state)
                : mBlockDataUnion{id, brightness, 0, state} {}

        explicit BlockData(uint32_t allData) { mBlockDataUnion.allData = allData; }

        bool operator==(const BlockData& rhs) const noexcept { return rhs.getData() == getData(); }

        bool operator!=(const BlockData& rhs) const noexcept { return !(*this == rhs); }

        [[nodiscard]] uint32_t getData() const noexcept { return mBlockDataUnion.allData; }

        [[nodiscard]] uint32_t getID() const noexcept { return mBlockDataUnion.data.id; }

        [[nodiscard]] uint32_t getBrightness() const noexcept { return mBlockDataUnion.data.sky; }

        [[nodiscard]] uint32_t getState() const noexcept { return mBlockDataUnion.data.state; }

        void setID(uint32_t id) noexcept { mBlockDataUnion.data.id = id; }

        void setBrightness(uint32_t brightness) noexcept { mBlockDataUnion.data.sky = brightness; }

        void setState(uint32_t state) noexcept { mBlockDataUnion.data.state = state; }
    private:
        union BlockDataUnion {
            struct BlockDataUnionStruct {
                uint32_t id : 16; // Block ID
                uint32_t sky : 4; // Brightness
                uint32_t block : 4; // Brightness
                uint32_t state : 8; // Block state
            } data;

            uint32_t allData;
        } mBlockDataUnion{};
    };

    class BlockType {
    public:
        BlockType(const char *name, bool solid, bool translucent, bool opaque, int hardness) :
                mName(name), mSolid(solid), mTranslucent(translucent), mOpaque(opaque), mHardness(hardness) {}

        [[nodiscard]] const std::string &getName() const noexcept { return mName; }

        [[nodiscard]] bool isSolid() const noexcept { return mSolid; }

        [[nodiscard]] bool isTranslucent() const noexcept { return mTranslucent; }

        [[nodiscard]] bool isOpaque() const noexcept { return mOpaque; }

        [[nodiscard]] int getHardness() const noexcept { return mHardness; }
    private:
        std::string mName;
        bool mSolid, mTranslucent, mOpaque;
        int mHardness;
    };

    class NWCOREAPI Blocks {
    public:
        Blocks();

        size_t registerBlock(const BlockType &block);

        const BlockType &operator[](size_t id) const { return mBlocks[id]; }

        static Blocks &getInstance();

    private:
        std::vector<BlockType> mBlocks;
    };
}

