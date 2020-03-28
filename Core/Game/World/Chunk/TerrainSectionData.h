#include <vector>
#include <cassert>
#include <iostream>
#include <unordered_set>
#include "Game/World/Blocks.h"
#include <Math/Vector3.h>
#include <set>

/*
 * A class that stores the blocks data (id, etc.) in a chunk
 * Uses a "palette", which is a map from small chunk-specific ids
 * (variable lengths: 4-bit, 8-bit or 32-bit) to the actual block data (BlockData class).
 * In case of 32-bit length, the palette is not used and id == BlockData
 */
class TerrainSectionData {
public:
    TerrainSectionData(int idLength = 32) : mIdLengthInBit(idLength) {
        assert(idLength == 4 || idLength == 8 || idLength == 32);
        allocateSpace();
    }

    [[nodiscard]] Game::World::BlockData getBlock(const Int3& pos) const noexcept {
        return getBlockDataFromId(getBlockIdByIndex(pos.X * ChunkSize * ChunkSize + pos.Y * ChunkSize + pos.Z, mChunkData, mIdLengthInBit));
    }

    void setBlock(const Int3& pos, Game::World::BlockData block) noexcept {
        auto id = getIdFromBlockData(block);
        if (id == -1) id = addNewBlockToPalette(block);
        setBlockIdByIndex(getIndexByPosition(pos), id, mChunkData, mIdLengthInBit);
    }

	// Find the appropriate length of id and compress data
    void optimize() noexcept {
        class BlockDataHash {
        public:
            size_t operator()(const Game::World::BlockData& block) const noexcept {
                return std::hash<size_t>()(block.getData());
            }
        };

        // TODO: this code needs optimization!
        std::unordered_set<Game::World::BlockData, BlockDataHash> newPalette;
        for (int x = 0; x < ChunkSize; x++) {
            for (int y = 0; y < ChunkSize; y++) {
                for (int z = 0; z < ChunkSize; z++) {
                    auto data = getBlock({ x, y, z});
                    if (newPalette.find(data) == newPalette.end()) newPalette.insert(data);
                }
            }
        }
        for (int x = 0; x < ChunkSize; x++) {
            for (int y = 0; y < ChunkSize; y++) {
                for (int z = 0; z < ChunkSize; z++) {
                    auto data = getBlock({ x, y, z });
                    auto id = std::distance(newPalette.begin(), newPalette.find(data));
                    setBlockIdByIndex(getIndexByPosition({ x,y,z }), id, mChunkData, mIdLengthInBit);
                }
            }
        }
        mPalette = std::vector(newPalette.begin(), newPalette.end());
        auto oldLength = mIdLengthInBit;
        if (newPalette.size() <= 16) mIdLengthInBit = 4;
        else if (newPalette.size() <= 256) mIdLengthInBit = 8;
        else mIdLengthInBit = 32;
		if(oldLength != mIdLengthInBit) {
            reallocateAndCopyData(oldLength, true);
		}
    }

    [[nodiscard]] size_t getBitLength() const noexcept { return mIdLengthInBit; }
private:
    static constexpr int BlocksSize = 32 * 32 * 32;
    static constexpr int ChunkSize = 32;
    //using InternalIDType = short;
    struct InternalIDType {
        union {
            struct {
                unsigned int length4_1 : 4;
                unsigned int length4_2 : 4;
                unsigned int length4_3 : 4;
                unsigned int length4_4 : 4;
                unsigned int length4_5 : 4;
                unsigned int length4_6 : 4;
                unsigned int length4_7 : 4;
                unsigned int length4_8 : 4;
            } length4Holder;
            struct {
                uint8_t length8_1;
                uint8_t length8_2;
                uint8_t length8_3;
                uint8_t length8_4;
            } length8Holder;
            uint32_t length32;
        };
    };

    static uint32_t getIndexByPosition(const Int3& pos) {
        return pos.X * ChunkSize* ChunkSize + pos.Y * ChunkSize + pos.Z;
    }

    [[nodiscard]] static uint32_t getBlockIdByIndex(size_t index, const std::unique_ptr<InternalIDType[]>& chunkData, size_t idLength) noexcept {
        size_t actualIndex = index * idLength / 32;
        size_t offset = index % (32 / idLength);
        InternalIDType ids = chunkData[actualIndex];
        switch (idLength) {
        case 4: {
            auto holder4 = ids.length4Holder;
            if (offset==0)return holder4.length4_1;
            if (offset==1)return holder4.length4_2;
            if (offset==2)return holder4.length4_3;
            if (offset==3)return holder4.length4_4;
            if (offset==4)return holder4.length4_5;
            if (offset==5)return holder4.length4_6;
            if (offset==6)return holder4.length4_7;
            if (offset==7)return holder4.length4_8;
        }
        case 8: {
            auto holder8 = ids.length8Holder;
            if (offset==0)return holder8.length8_1;
            if (offset==1)return holder8.length8_2;
            if (offset==2)return holder8.length8_3;
            if (offset==3)return holder8.length8_4;
        }
        case 32:
            return ids.length32;
        }
        assert(false); // shouldn't reach here
        return 0;
    }

    static void setBlockIdByIndex(size_t index, uint32_t id, std::unique_ptr<InternalIDType[]>& chunkData, size_t idLength) noexcept {
        size_t actualIndex = index * idLength / 32;
        size_t offset = index % (32 / idLength);
        InternalIDType& ids = chunkData[actualIndex];
        if (idLength == 4) {
            auto& holder4 = ids.length4Holder;
            if (offset == 0) holder4.length4_1 = id;
            else if (offset == 1) holder4.length4_2 = id;
            else if (offset == 2) holder4.length4_3 = id;
            else if (offset == 3) holder4.length4_4 = id;
            else if (offset == 4) holder4.length4_5 = id;
            else if (offset == 5) holder4.length4_6 = id;
            else if (offset == 6) holder4.length4_7 = id;
            else if (offset == 7) holder4.length4_8 = id;
            return;
        }
        if (idLength == 8) {
            auto& holder8 = ids.length8Holder;
            if (offset == 0) holder8.length8_1 = id;
            else if (offset == 1) holder8.length8_2 = id;
            else if (offset == 2) holder8.length8_3 = id;
            else if (offset == 3) holder8.length8_4 = id;
            return;
        }
        if (idLength == 32) {
            ids.length32 = id;
            return;
        }
    }

	Game::World::BlockData getBlockDataFromId(uint32_t id) const noexcept {
        if (mIdLengthInBit == 32) return Game::World::BlockData(id);
        assert(id < mPalette.size());
        return mPalette[id];
	}

	// Return (uint32_t)-1 if not found
	uint32_t getIdFromBlockData(Game::World::BlockData data) {
        if (mIdLengthInBit == 32) return data.getData();
        for (size_t i = 0; i < mPalette.size(); i++) {
            if (mPalette[i] == data) return i;
        }
        return -1;
    }

    size_t addNewBlockToPalette(Game::World::BlockData block) {
        mPalette.push_back(block);
        if (mPalette.size() > getPaletteCapacity()) {
            if (mIdLengthInBit == 4) {
                mIdLengthInBit = 8;
                reallocateAndCopyData(4);
            }
            else if (mIdLengthInBit == 8) {
                mIdLengthInBit = 32;
                reallocateAndCopyData(8);
            }
        }
		
        return mPalette.size() - 1;
    }

    /* Reallocate the block data space and copy over the old id data.
    // mIdLengthInBit should be set to the target size before calling this function.
    // plainCopy: When true, even if new bit is 32, the ids are copied as-is without converting.
    */
    void reallocateAndCopyData(size_t oldLength, bool plainCopy = false) {
        auto oldSpace = allocateSpace();
        if (mIdLengthInBit == 32 && !plainCopy) { // Since palette is not used in this case, the old ids need to be converted into block data.
            for (int x = 0; x < 32; x++)
                for (int y = 0; y < 32; y++)
                    for (int z = 0; z < 32; z++)
                        setBlock({ x,y,z }, mPalette[getBlockIdByIndex(getIndexByPosition({ x,y,z }), oldSpace, oldLength)]);
        }
        else {
            for (size_t i = 0; i < BlocksSize; i++)
                setBlockIdByIndex(i, getBlockIdByIndex(i, oldSpace, oldLength), mChunkData, mIdLengthInBit);
        }
    }

    [[nodiscard]] size_t getPaletteCapacity() const noexcept {
        if (mIdLengthInBit == 4) return (1 << 4);
        if (mIdLengthInBit == 8) return (1 << 8);
        return std::numeric_limits<size_t>::max();
    }

	// Allocate mChunkData according to the bitLength. Return the pointer to the old data.
    std::unique_ptr<InternalIDType[]> allocateSpace() {
        auto ret = std::move(mChunkData);
        mChunkData = std::make_unique<InternalIDType[]>(size_t(std::ceil(BlocksSize * mIdLengthInBit / 32)));
        return ret;
	}

    int mIdLengthInBit; // the length of chunk-specific id
    
    std::unique_ptr<InternalIDType[]> mChunkData;
    std::vector<Game::World::BlockData> mPalette;
};
