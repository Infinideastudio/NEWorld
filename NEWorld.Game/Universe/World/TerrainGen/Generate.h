#pragma once

#include "Chunk.h"
#include "Heights.h"

namespace World::TerrainGen {
    class Generate {
    public:
        explicit Generate(int seed) : mHeights(seed) {
            Cursor(Int3{0}, Int3{15}, [this](auto p) noexcept {
                mSkyChunk.SetBlock(p, Blocks::ENV);
                mSkyChunk.SetBrightness(p, skylight);
                mLavaChunk.SetBlock(p, Blocks::LAVA);
                mLavaChunk.SetBrightness(p, skylight);
            });
        }

        // TODO(remove when system is completed)
        static Generate &Get() {
            static Generate instance{3404};
            return instance;
        }

        Chunk *Run(Int3 c) {
            auto result = Draft(c);
            // TODO(this is a hack, formalize it)
            if (result->RawUnsafe() == &mSkyChunk) result->Empty = true;
            if (!result->Empty) result->updated = true;
            return result;
        }

    private:
        Heights mHeights;
        ChunkData mSkyChunk{}, mLavaChunk{};

        static ChunkData *NoiseTerrain(Int3 pos, Heights::Section &hm) {
            auto[cx, cy, cz] = pos.Data;
            auto result = std::make_unique<ChunkData>();
            int maxh;

            const auto sh = WaterLevel + 2 - (cy << 4);
            const auto wh = WaterLevel - (cy << 4);

            for (auto x = 0; x < 16; ++x) {
                for (auto z = 0; z < 16; ++z) {
                    const auto h = hm.Get(x, z) - (cy << 4);
                    if (h > sh && h > wh + 1) {
                        //Grass layer
                        if (h >= 0 && h < 16) result->SetBlock(Int3{x, h, z}, Blocks::GRASS);
                        //Dirt layer
                        maxh = std::min(std::max(0, h), 16);
                        for (auto y = std::min(std::max(0, h - 5), 16); y < maxh; ++y)
                            result->SetBlock(Int3{x, y, z}, Blocks::DIRT);
                    } else {
                        //Sand layer
                        maxh = std::min(std::max(0, h + 1), 16);
                        for (auto y = std::min(std::max(0, h - 5), 16); y < maxh; ++y)
                            result->SetBlock(Int3{x, y, z}, Blocks::SAND);
                        //Water layer
                        const auto minh = std::min(std::max(0, h + 1), 16);
                        maxh = std::min(std::max(0, wh + 1), 16);
                        auto cur_br = BRIGHTNESSMAX - (WaterLevel - (maxh - 1 + (cy << 4))) * 2;
                        if (cur_br < BRIGHTNESSMIN) cur_br = BRIGHTNESSMIN;
                        for (auto y = maxh - 1; y >= minh; --y) {
                            result->SetBlock(Int3{x, y, z}, Blocks::WATER);
                            result->SetBrightness(Int3{x, y, z}, static_cast<Brightness>(cur_br));
                            cur_br -= 2;
                            if (cur_br < BRIGHTNESSMIN) cur_br = BRIGHTNESSMIN;
                        }
                    }
                    //Rock layer
                    maxh = std::min(std::max(0, h - 5), 16);
                    for (auto y = 0; y < maxh; ++y) result->SetBlock(Int3{x, y, z}, Blocks::ROCK);
                    //Air layer
                    for (auto y = std::min(std::max(0, std::max(h + 1, wh + 1)), 16); y < 16; ++y) {
                        result->SetBlock(Int3{x, y, z}, Blocks::ENV);
                        result->SetBrightness(Int3{x, y, z}, skylight);
                    }
                }
            }
            return result.release();
        }

        static Chunk* Make(Int3 pos, ChunkData *data, bool isShared, std::shared_ptr<PmrBase> hm) {
            auto result = new Chunk(pos, data, isShared);
            result->Attach(std::move(hm));
            return result;
        }

        Chunk * Draft(Int3 pos) {
            auto[cx, cy, cz] = pos.Data;
            //Fast generate parts
            //Part1 out of the terrain bound
            if (cy > 4) return new Chunk(pos, &mSkyChunk, true);
            if (cy < 0) return new Chunk(pos, &mLavaChunk, true);

            //Part2 out of geometry area
            auto cur = mHeights.Get(cx, cz); // TODO(attach this to chunk)
            if (cy > cur->High()) return Make(pos, &mSkyChunk, true, std::move(cur));
            if (cy < cur->Low()) {
                auto result = std::make_unique<ChunkData>();
                Cursor(Int3{0}, Int3{15}, [&result](auto p) noexcept {
                    result->SetBlock(p, Blocks::ROCK);
                    result->SetBrightness(p, 0);
                });
                return Make(pos, result.release(), false, std::move(cur));
            }
            const auto terrain = NoiseTerrain(pos, *cur);
            return Make(pos, terrain, false, std::move(cur));
        }

    };
}
