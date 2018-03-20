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

class BlockData {
public:
    BlockData() : u{{0}} { }

    BlockData(uint32_t id_, uint32_t brightness_, uint32_t state_)
        : u{id_, brightness_, state_} { }

    BlockData(uint32_t allData) { u.allData = allData; }

    uint32_t getData() const { return u.allData; }
    uint32_t getID() const { return u.data.id; }

    uint32_t getBrightness() const { return u.data.brightness; }

    uint32_t getState() const { return u.data.state; }

    void setID(uint32_t id_) { u.data.id = id_; }

    void setBrightness(uint32_t brightness_) { u.data.brightness = brightness_; }

    void setState(uint32_t state_) { u.data.state = state_; }

private:
    union BlockDataUnion {
        struct BlockDataUnionStruct {
            uint32_t id : 12; // Block ID
            uint32_t brightness : 4; // Brightness
            uint32_t state : 16; // Block state
        } data;

        uint32_t allData;
    } u;
};

class BlockType {
private:
    std::string mName;
    bool mSolid;
    bool mTranslucent;
    bool mOpaque;
    int mExplodePower;
    int mHardness;

public:
    BlockType(const char* name_, bool solid_, bool translucent_, bool opaque_, int explodePower_, int hardness_) :
        mName(name_), mSolid(solid_), mTranslucent(translucent_), mOpaque(opaque_), mExplodePower(explodePower_),
        mHardness(hardness_) { }

    // Internal block name
    const std::string& getName() const { return mName; }

    // Is solid block
    bool isSolid() const { return mSolid; }

    // Transparency determines how it will rendered
    bool isTranslucent() const { return mTranslucent; }

    // Opaque means it blocks light
    bool isOpaque() const { return mOpaque; }

    // Explode power, if it isn't a explosive set this to 0
    int getExplodePower() const { return mExplodePower; }

    // Hardness
    int getHardness() const { return mHardness; }
};

class NWCOREAPI BlockManager {
public:
    BlockManager();
    void showInfo(size_t id) const;
    size_t registerBlock(const BlockType& block);

    const BlockType& operator[](size_t id) const { return mBlocks[id]; }

private:
    std::vector<BlockType> mBlocks;
};
