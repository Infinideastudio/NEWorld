#pragma once

#include "Noise.h"
#include <bit>
#include <atomic>
#include <kls/Object.h>
#include <tsl/hopscotch_map.h>

namespace World::TerrainGen {
    class Heights {
    public:
        class Section : public kls::PmrBase {
        public:
            void Init(Noise &noise, int cx, int cz) {
                std::call_once(mLazy, [this, &noise, cx, cz]() {
                    auto low = std::numeric_limits<int>::max(), high = WaterLevel;
                    const int bX = cx * 16, bZ = cz * 16;
                    for (auto x = 0; x < 16; ++x) {
                        for (auto z = 0; z < 16; ++z) {
                            const auto height = mHeights[x][z] = noise.Get(x + bX, z + bZ);
                            if (height < low) low = height;
                            if (height > high) high = height;
                        }
                    }
                    mCLow = static_cast<int>(std::floor(double(low) / 16.0));
                    mCHigh = static_cast<int>(std::ceil(double(high) / 16.0));
                });
            }

            [[nodiscard]] int Low() const noexcept { return mCLow; }

            [[nodiscard]] int High() const noexcept { return mCHigh; }

            [[nodiscard]] int Get(int x, int z) const noexcept { return mHeights[x][z]; }

        private:
            int mCLow{}, mCHigh{};
            int mHeights[16][16]{};
            std::once_flag mLazy{};
        };

        explicit Heights(int seed) : mNoise(seed) {}

        std::shared_ptr<Section> Get(int cx, int cz) {
            auto result = GetEntry(cx, cz);
            result->Init(mNoise, cx, cz);
            return result;
        }
    private:
        Noise mNoise;
        std::mutex mMutex;
        // TODO(Add a cleanup tick)
        tsl::hopscotch_map<uint64_t, std::weak_ptr<Section>> mSections{};

        static uint64_t MakeKey(int32_t cx, int32_t cz) noexcept {
            const auto uCx = std::bit_cast<uint32_t>(cx);
            const auto uCz = std::bit_cast<uint32_t>(cz);
            return (static_cast<uint64_t>(uCx) << 32ull) | static_cast<uint64_t>(uCz);
        }

        std::shared_ptr<Section> GetEntry(int cx, int cz) {
            std::lock_guard lk{mMutex};
            const auto key = MakeKey(cx, cz);
            const auto find = mSections.find(key);
            if (find != mSections.end()) {
                auto current = find.value().lock();
                if (current) return current;
            }
            auto result = std::make_shared<Section>();
            mSections[key] = result;
            return result;
        }
    };
}
