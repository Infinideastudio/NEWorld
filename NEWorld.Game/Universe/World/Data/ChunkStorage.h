#pragma once

#include <array>
#include <vector>
#include <algorithm>
#include "BitStorage.h"

namespace World {
    constexpr unsigned int ChunkEdgeSizeLog2 = 4;
    constexpr unsigned int ChunkPlaneSizeLog2 = ChunkEdgeSizeLog2 * 2u;
    constexpr unsigned int ChunkCubicSizeLog2 = ChunkEdgeSizeLog2 * 3u;
    constexpr unsigned int ChunkEdgeSize = 1u << ChunkEdgeSizeLog2;
    constexpr unsigned int ChunkPlaneSize = 1u << ChunkPlaneSizeLog2;
    constexpr unsigned int ChunkCubicSize = 1u << ChunkCubicSizeLog2;
}

namespace World::Data {
    constexpr int SIZE = World::ChunkCubicSize;

    struct InitializeT {
    };

    constexpr InitializeT Init;

    class BlocksSparse4 {
        struct Split {
            uint8_t upper: 4;
            uint8_t lower: 4;
        };
    public:
        BlocksSparse4() = default;

        explicit BlocksSparse4(InitializeT) noexcept
                : mStorage(SIZE >> 1, {0, 0}) {}

        [[nodiscard]] int Get(const int index) const noexcept {
            const auto sec = mStorage[index >> 1u]; // NOLINT
            return (index & 1) ? sec.upper : sec.lower; // NOLINT
        }

        void Set(const int index, const int val) noexcept {
            auto &sec = mStorage[index >> 1u]; // NOLINT
            if (index & 1) sec.upper = val; else sec.lower = val; // NOLINT
        }

        [[nodiscard]] auto Raw() noexcept { return mStorage.data(); }

        [[nodiscard]] auto Raw() const noexcept { return mStorage.data(); }

    private:
        std::vector<Split> mStorage;
    };

    class BlocksSparse8 {
    public:
        BlocksSparse8() = default;

        explicit BlocksSparse8(InitializeT) noexcept
                : mStorage(SIZE, 0) {}

        [[nodiscard]] int Get(const int index) const noexcept { return mStorage[index]; }

        void Set(const int index, const int val) noexcept { mStorage[index] = val; }

        [[nodiscard]] auto Raw() noexcept { return mStorage.data(); }

        [[nodiscard]] auto Raw() const noexcept { return mStorage.data(); }

    private:
        std::vector<uint8_t> mStorage;
    };

    class BlocksSparse16 {
    public:
        BlocksSparse16() = default;

        explicit BlocksSparse16(InitializeT) noexcept
                : mStorage(SIZE, 0) {}

        [[nodiscard]] int Get(const int index) const noexcept { return mStorage[index]; }

        void Set(const int index, const int val) noexcept { mStorage[index] = val; }

        [[nodiscard]] auto Raw() noexcept { return mStorage.data(); }

        [[nodiscard]] auto Raw() const noexcept { return mStorage.data(); }

    private:
        std::vector<uint16_t> mStorage;
    };

    class BlockPalette4 {
    public:
        uintptr_t toVal(const int id) const noexcept { return ((id < mSize) ? mT[id] : 0); }

        [[nodiscard]] int tryFromVal(const uintptr_t val) const noexcept {
            for (int i = 0; i < mSize; ++i) { if (mT[i] == val) return i; }
            return -1;
        }

        int fromVal(const uintptr_t val) noexcept {
            if (const auto v = tryFromVal(val); v == -1) {
                return (mSize < 16) ? (mT[mSize] = val, mSize++) : -1;
            } else return v;
        }

        [[nodiscard]] auto Size() const noexcept { return mSize; }

        [[nodiscard]] auto Raw() const noexcept { return mT.get(); }

    private:
        std::unique_ptr<uintptr_t[]> mT{new uintptr_t[16]};
        int mSize{0};
    };

    class BlockPalette8 {
        static constexpr int LdRev(int size) noexcept { return (size >> 2) * 5; } // NOLINT, 0.8 Load Factor
        static constexpr uintptr_t EmptyVal = ~uintptr_t(0);
    public:
        explicit BlockPalette8(int bits = 5) noexcept: mSize(0), mCurMax(1u << bits), mCurMask(mCurMax - 1) { // NOLINT
            StgCreate();
        }

        explicit BlockPalette8(const BlockPalette4 &p4) noexcept: BlockPalette8(5) {
            mSize = p4.Size();
            std::memcpy(mT.get(), p4.Raw(), p4.Size() * sizeof(uintptr_t));
            ReHash();
        }

        uintptr_t toVal(const int id) const noexcept { return ((id < mSize) ? mT[id] : 0); }

        [[nodiscard]] int tryFromVal(const uintptr_t val) const noexcept {
            for (auto i = Hash(val);; ++i) {
                const auto m = i & mCurMask; // NOLINT
                const auto v = rLookUp[m];
                if (v == val) return rMap[m];
                if (v == EmptyVal) return -1;
            }
        }

        int fromVal(const uintptr_t val) noexcept {
            if (mSize == mCurMax) return -1;
            for (auto i = Hash(val);; ++i) {
                const auto m = i & mCurMask; // NOLINT
                const auto v = rLookUp[m];
                if (v == val) return rMap[m];
                if (v == EmptyVal) return (rLookUp[m] = val, mT[mSize] = val, rMap[m] = mSize++);
            }
        }

        void UpScale() noexcept {
            mCurMax <<= 1; // NOLINT
            mCurMask = mCurMax - 1;
            auto s = std::move(mT);
            StgCreate();
            std::memcpy(mT.get(), s.get(), mSize * sizeof(uintptr_t));
            ReHash();
        }

    private:
        [[nodiscard]] uint8_t Hash(const uint16_t v) const noexcept { return v & mCurMask; } // NOLINT

        void ReHash() noexcept {
            for (int i = 0; i < mSize; ++i) {
                for (auto u = Hash(mT[i]);; ++u) {
                    const auto m = u & mCurMask; // NOLINT
                    const auto v = rLookUp[m];
                    if (v == EmptyVal) {
                        (rLookUp[m] = mT[i], rMap[m] = i);
                        break;
                    }
                }
            }
        }

        void StgCreate() noexcept {
            mT = std::unique_ptr<uintptr_t[]>(new uintptr_t[mCurMax + LdRev(mCurMax) + LdRev(mCurMax >> 1)]); // NOLINT
            rMap = reinterpret_cast<uint8_t *>(mT.get() + mCurMax + LdRev(mCurMax));
            rLookUp = mT.get() + mCurMax;
            std::memset(rLookUp, 0xFF, LdRev(mCurMax) * (sizeof(uintptr_t) + sizeof(uint8_t)));
        }

        std::unique_ptr<uintptr_t[]> mT{};
        uintptr_t *rLookUp{};
        uint8_t *rMap{};
        int mSize, mCurMax, mCurMask;
    };

    // TODO(Need to be worked on)
    class BlockPalette16 {
    public:
        uintptr_t toVal(const int id) const noexcept { return id; } // NOLINT

        [[nodiscard]] int tryFromVal(const uintptr_t val) const noexcept { return val; } // NOLINT

        int fromVal(const uintptr_t val) noexcept { return tryFromVal(val); } // NOLINT
    };

    class ChunkStorage {
    public:
        explicit ChunkStorage(int bits) noexcept;

        ChunkStorage(int bits, InitializeT) noexcept;

        ~ChunkStorage() noexcept;

        [[nodiscard]] int Get(const int index) const noexcept {
            switch (mT) {
                case 0:
                    return mP.p4.toVal(mV.b4.Get(index));
                case 1:
                    return mP.p8.toVal(mV.b8.Get(index));
                case 2:
                    return mP.p16.toVal(mV.b16.Get(index));
            }
            return 0;
        }

        void Set(int index, uintptr_t val) noexcept;

    private:
        bool TrySet(int index, uintptr_t val) noexcept;

        void Scale48() noexcept;

        void Scale88() noexcept;

        void Scale8H() noexcept;

        void UpScale() noexcept;

        static constexpr int CheckMode(int bits) noexcept {
            if (bits <= 4) return 0;
            if (bits <= 8) return 1;
            return 2;
        }

        int mT, mBit;

        union V {
            V() noexcept {} // NOLINT
            ~V() noexcept {} // NOLINT
            BlocksSparse4 b4;
            BlocksSparse8 b8;
            BlocksSparse16 b16;
        } mV;

        union P {
            P() noexcept {} // NOLINT
            ~P() noexcept {} // NOLINT
            BlockPalette4 p4;
            BlockPalette8 p8;
            BlockPalette16 p16;
        } mP;
    };
}
