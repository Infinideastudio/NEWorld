// 
// MainPlugin: worldgen.cpp
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

#include <cmath>
#include <Math/Vector3.h>
#include <Game/World/Chunk/Chunk.h>
#include "worldgen.h"

int WorldGen::seed = 1025;
double WorldGen::NoiseScaleX = 64;
double WorldGen::NoiseScaleZ = 64;

extern int32_t GrassID, RockID, DirtID, SandID, WaterID;

namespace {
    constexpr double noise(const int x, const int y) noexcept {
        long long xx = x * 107 + y * 13258953287;
        xx = xx >> 13 ^ xx; //NOLINT
        return (xx * (xx * xx * 15731 + 789221) + 1376312589 & 0x7fffffff) / 16777216.0; //NOLINT
    }

    constexpr double interpolate(const double a, const double b, const double x) noexcept {
        return a * (1.0 - x) + b * x;
    }

    double interpolatedNoise(const double x, const double y) noexcept {
        const auto intX = int(floor(x));
        const auto intY = int(floor(y));
        const auto v1 = noise(intX, intY);
        const auto v2 = noise(intX+1, intY);
        const auto v3 = noise(intX, intY+1);
        const auto v4 = noise(intX+1, intY+1);
        const double fractional_X = x - intX;
        const double fractional_Y = y - intY;
        const auto i1 = interpolate(v1, v2, fractional_X);
        const auto i2 = interpolate(v3, v4, fractional_X);
        return interpolate(i1, i2, fractional_Y);
    }

    double perlinNoise2D(const double x, const double y) noexcept {
        double total = 0, frequency = 1, amplitude = 1;
        for (int i = 0; i <= 4; i++) {
            total += interpolatedNoise(x*frequency, y*frequency) * amplitude;
            frequency *= 2;
            amplitude /= 2.0;
        }
        return total;
    }

    int getHeight(const int x, const int y) noexcept {
        return int(perlinNoise2D(x/WorldGen::NoiseScaleX, y/WorldGen::NoiseScaleZ)) / 2 - 64;
    }

    constexpr auto ChunkSize = Chunk::Size();
    constexpr auto SandHeight = 2;
    constexpr auto WaterLevel = 0;

    using HeightMap = int[ChunkSize][ChunkSize];

    std::pair<int, int> getHeightMap(const Int3& pos, HeightMap& heights) noexcept {
        int high = std::numeric_limits<int>::min(), low = std::numeric_limits<int>::max();
        for (int x = 0; x < ChunkSize; ++x) {
            for (int z = 0; z < ChunkSize; ++z) {
                const auto absHeight = getHeight(pos.X * ChunkSize + x, pos.Z * ChunkSize + z);
                heights[x][z] = absHeight;
                const auto height = absHeight - pos.Y * ChunkSize;
                if (height > high) high = height;
                if (height < low) low = height;
            }
        }
        return {high, low};
    }

    bool checkMonotonicFill(const ChunkGenerateArgs& args, const std::pair<int, int>& summary) noexcept {
        const auto& [high, low] = summary;
        if (low - 3 >= ChunkSize) {
            args.chunk->setMonotonic({ static_cast<uint32_t>(RockID), 0,0 });
            return true;
        }
        if (high < 0 && args.pos->Y * ChunkSize > 0) {
            args.chunk->setMonotonic({ 0, static_cast<uint32_t>(args.skyLight),0 });
            return true;
        }
        return false;
    }

    void genChunkDetails(const ChunkGenerateArgs* args, HeightMap& heightMap) noexcept {
        auto& blocks = *args->chunk->getBlocks();
        for (int x = 0; x < ChunkSize; x++)
            for (int z = 0; z < ChunkSize; z++) {
                const auto absHeight = heightMap[x][z];
                const auto height = absHeight - args->pos->Y * ChunkSize;
                const bool underWater = absHeight <= WaterLevel;
                for (int y = 0; y < ChunkSize; y++) {
                    auto& block = blocks[x * ChunkSize * ChunkSize + y * ChunkSize + z];
                    if (y <= height) {
                        if (absHeight < SandHeight) { block.setID(SandID); }
                        else if (y == height) { block.setID((underWater) ? SandID : GrassID); }
                        else if (y >= height - 3) { block.setID((underWater) ? SandID : DirtID); }
                        else { block.setID(RockID); }
                        block.setBrightness(0);
                    }
                    else if (const auto by = args->pos->Y * ChunkSize + y; by <= 0) {
                        block.setID(WaterID);
                        block.setBrightness(std::max(args->skyLight + (by / 2), 0));
                    } else {
                        block.setID(0);
                        block.setBrightness(args->skyLight);
                    }
                    block.setState(0);
                }
            }
    }

    void generator(const ChunkGenerateArgs* args) {
        HeightMap heightMap {};
        if (!checkMonotonicFill(*args, getHeightMap(*args->pos, heightMap))) {
            args->chunk->allocateBlocks(false);
            genChunkDetails(args, heightMap);
        }
    }
}

void WorldGen::selfRegister() { nwRegisterChunkGenerator(generator); }
