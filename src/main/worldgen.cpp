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
#include <iostream>
#include "worldgen.h"

int WorldGen::seed = 1025;
double WorldGen::NoiseScaleX = 64;
double WorldGen::NoiseScaleZ = 64;

extern int32_t GrassID, RockID, DirtID, SandID, WaterID;

// Chunk generator
void NWAPICALL generator(const NWvec3i* pos, NWblockdata* blocks, int daylightBrightness) {
    for (int x = 0; x < NWChunkSize; x++)
        for (int z = 0; z < NWChunkSize; z++) {
            int absHeight = WorldGen::getHeight(pos->x * NWChunkSize + x, pos->z * NWChunkSize + z);
            int height = absHeight - pos->y * NWChunkSize;
            bool underWater = (absHeight) <= 0;
            for (int y = 0; y < NWChunkSize; y++) {
                NWblockdata& block = blocks[x * NWChunkSize * NWChunkSize + y * NWChunkSize + z];
                if (y <= height) {
                    if (y == height) { block.id = ((underWater) ? SandID : GrassID); }
                    else if (y >= height - 3) { block.id = ((underWater) ? SandID : DirtID); }
                    else { block.id = RockID; }
                    block.brightness = block.state = 0;
                }
                else {
                    block.id = ((pos->y * NWChunkSize + y <= 0) ? WaterID : NWAirID);
                    block.brightness = daylightBrightness;
                    block.state = 0;
                }
            }
        }
}

double WorldGen::InterpolatedNoise(double x, double y) {
    int int_X, int_Y;
    double fractional_X, fractional_Y, v1, v2, v3, v4, i1, i2;
    int_X = int(floor(x));
    fractional_X = x - int_X;
    int_Y = int(floor(y));
    fractional_Y = y - int_Y;
    v1 = Noise(int_X, int_Y);
    v2 = Noise(int_X + 1, int_Y);
    v3 = Noise(int_X, int_Y + 1);
    v4 = Noise(int_X + 1, int_Y + 1);
    i1 = Interpolate(v1, v2, fractional_X);
    i2 = Interpolate(v3, v4, fractional_X);
    return Interpolate(i1, i2, fractional_Y);
}

double WorldGen::PerlinNoise2D(double x, double y) {
    double total = 0, frequency = 1, amplitude = 1;
    for (int i = 0; i <= 4; i++) {
        total += InterpolatedNoise(x * frequency, y * frequency) * amplitude;
        frequency *= 2;
        amplitude /= 2.0;
    }
    return total;
}
