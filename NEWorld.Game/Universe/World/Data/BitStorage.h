#pragma once

#include <cmath>
#include <memory>
#include <cstddef>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <vector>

namespace World::Data {
    template<class T, class U>
    T Exchange(T &v, const U as) noexcept {
        const auto r = v;
        v = as;
        return r;
    }

    template<class T, class U, class V>
    void AssertBetween(const T low, const U high, const V val) noexcept { assert(val >= low && val <= high); }

    class BitsDenseView {
    public:
        BitsDenseView(const unsigned int bits, const unsigned int size, uint64_t *const data)
                : mMask((1ull << bits) - 1ull), mData(data),
                  mBits(bits), mSize(size),
                  mDataSize(static_cast<unsigned int>(ceil(double(size * bits) / 64.0) * sizeof(uint64_t))) {
            AssertBetween(1u, 32u, bits);
        }

        BitsDenseView(const BitsDenseView &r) noexcept = default;

        BitsDenseView(BitsDenseView &&r) noexcept
                : BitsDenseView(r) { r.mData = nullptr; } // NOLINT

        BitsDenseView &operator=(const BitsDenseView &r) noexcept = default;

        BitsDenseView &operator=(BitsDenseView &&r) noexcept {
            *this = r, r.mData = nullptr;
            return *this;
        }

        void Set(const unsigned int index, const uint64_t val) noexcept {
            AssertBetween(0u, mSize - 1, index);
            AssertBetween(0u, mMask, val);
            const auto bitOffset = index * mBits;
            const auto hByteBegin = bitOffset >> 6u;
            const auto hByteEnd = (bitOffset + (mBits - 1u)) >> 6u;
            const auto headBitLoc = bitOffset & 63u;
            const auto lowerRemains = mData[hByteBegin] & ~(mMask << headBitLoc);
            const auto lowerChanged = (val & mMask) << headBitLoc;
            mData[hByteBegin] = lowerRemains | lowerChanged;
            if (hByteBegin != hByteEnd) {
                const auto bitsLower = 64u - headBitLoc;
                const auto bitsUpper = mBits - bitsLower;
                const auto upperRemains = (mData[hByteEnd] >> bitsUpper) << bitsUpper;
                const auto upperChanged = (val & mMask) >> bitsLower;
                mData[hByteEnd] = upperRemains | upperChanged;
            }
        }

        [[nodiscard]] unsigned int Get(const unsigned int index) noexcept {
            AssertBetween(0u, mSize - 1, index);
            const auto bitOffset = index * mBits;
            const auto hByteBegin = bitOffset >> 6u;
            const auto hByteEnd = (bitOffset + (mBits - 1u)) >> 6u;
            const auto headBitLoc = bitOffset & 63u;
            if (hByteBegin == hByteEnd) {
                return static_cast<unsigned int>((mData[hByteBegin] >> headBitLoc) & mMask);
            } else {
                const auto bitsLower = 64u - headBitLoc;
                const auto upper = (mData[hByteEnd] << bitsLower) & mMask;
                const auto lower = mData[hByteBegin] >> headBitLoc;
                return static_cast<unsigned int>(upper | lower);
            }
        }

        template<class T>
        void Pack(const T *sparse) noexcept {
            uint64_t current = 0u;
            auto bS = 0u;
            auto bE = mBits;
            auto write = mData;
            for (const auto sE = sparse + mSize; sparse != sE; ++sparse) {
                const auto v = (*sparse) & mMask;
                current |= static_cast<uint64_t>(v) << bS;
                if (bE >= 64u) {
                    *(write++) = current;
                    current = (bE -= 64u) ? v >> (64u - bS) : 0u;
                }
                bS = bE;
                bE += mBits;
            }
            if (bS) { *write = current; }
        }

        template<class T>
        void Unpack(T *sparse) noexcept {
            auto bS = 0u;
            auto bE = mBits;
            auto write = mData;
            uint64_t current = *(write++);
            for (const auto sE = sparse + mSize; sparse != sE; ++sparse) {
                *sparse = (current >> bS) & mMask;
                if (bE >= 64u) {
                    bE -= 64u;
                    current = *(write++);
                    *sparse |= (current << (64u - bS)) & mMask;
                }
                bS = bE;
                bE += mBits;
            }
        }

        [[nodiscard]] auto Bits() const noexcept { return mBits; }

        [[nodiscard]] auto Size() const noexcept { return mSize; }

        [[nodiscard]] auto Raw() noexcept { return mData; }

        [[nodiscard]] auto Raw() const noexcept { return mData; }

        [[nodiscard]] auto RawSize() const noexcept { return mDataSize; }

    private:
        uint64_t mMask;
        uint64_t *mData;
        unsigned int mBits, mSize, mDataSize;
    };

    class BitsSparseView {
    public:
        BitsSparseView(const int bits, const int size, std::byte *const data) noexcept
                : mFlag(GetFlag(bits)), mSize(size), mData(data) {} // NOLINT

        BitsSparseView(const BitsSparseView &r) noexcept = default;

        BitsSparseView(BitsSparseView &&r) noexcept
                : BitsSparseView(r) { r.mData = nullptr; } // NOLINT

        BitsSparseView &operator=(const BitsSparseView &r) noexcept = default;

        BitsSparseView &operator=(BitsSparseView &&r) noexcept {
            *this = r, r.mData = nullptr;
            return *this;
        }

        void Set(const unsigned int index, const int val) noexcept {
            switch (mFlag) {
                case 0:
                    A<uint8_t>(index) = val;
                    return;
                case 1:
                    A<uint16_t>(index) = val;
                    return;
                case 2:
                    A<uint32_t>(index) = val;
            }
        }

        [[nodiscard]] int Get(const unsigned int index) const noexcept {
            switch (mFlag) {
                case 0:
                    return A<uint8_t>(index);
                case 1:
                    return A<uint16_t>(index);
                case 2:
                    return A<uint32_t>(index);
            }
            return 0;
        }

        [[nodiscard]] int GetSet(const unsigned int index, const int val) noexcept {
            switch (mFlag) {
                case 0:
                    return Exchange(A<uint8_t>(index), val);
                case 1:
                    return Exchange(A<uint16_t>(index), val);
                case 2:
                    return Exchange(A<uint32_t>(index), val);
            }
            return 0;
        }

        [[nodiscard]] auto Size() const noexcept { return mSize; }

        [[nodiscard]] auto Raw() noexcept { return mData; }

        [[nodiscard]] auto Raw() const noexcept { return mData; }

        [[nodiscard]] auto MaxBits() const noexcept { return 8 * (1 << mFlag); } // NOLINT
        [[nodiscard]] auto RawSize() const noexcept { return mSize << mFlag; } // NOLINT
    protected:
        static constexpr int GetFlag(const int bits) noexcept {
            if (bits <= 8) return 0;
            if (bits <= 16) return 1;
            return 2;
        }

    private:
        template<class T>
        [[nodiscard]] T &A(const unsigned int x) noexcept { return reinterpret_cast<T *>(mData)[x]; }

        template<class T>
        [[nodiscard]] const T &A(const unsigned int x) const noexcept { return reinterpret_cast<const T *>(mData)[x]; }

        static_assert(alignof(std::max_align_t) >= 4);
        static_assert(sizeof(int) >= 4);

        int mFlag, mSize;
        std::byte *mData;
    };

    class BitsDense : public BitsDenseView {
    public:
        BitsDense(const unsigned int bits, const unsigned int size, const bool init)
                : BitsDenseView(bits, size, new uint64_t[size_t(ceil(double(size * bits) / 64.0))]) {
            if (init) std::memset(Raw(), 0, RawSize());
        }

        BitsDense(const BitsDense &) = delete;

        BitsDense(BitsDense &&r) noexcept
                : BitsDenseView(std::move(r)) {}

        BitsDense &operator=(const BitsDense &) = delete;

        BitsDense &operator=(BitsDense &&r) noexcept {
            if (std::addressof(r) != this) {
                delete[] Raw();
                static_cast<BitsDenseView &>(*this) = static_cast<BitsDenseView &&>(std::move(r));
            }
            return *this;
        }

        ~BitsDense() noexcept { delete[] Raw(); }
    };

    class BitsSparse : public BitsSparseView {
    public:
        BitsSparse(const int bits, const int size, const bool init)
                : BitsSparseView(bits, size, new std::byte[size << GetFlag(bits)]) { // NOLINT
            if (init) std::memset(Raw(), 0, RawSize());
        }

        BitsSparse(const BitsSparse &) = delete;

        BitsSparse(BitsSparse &&r) noexcept
                : BitsSparseView(std::move(r)) {}

        BitsSparse &operator=(const BitsSparse &) = delete;

        BitsSparse &operator=(BitsSparse &&r) noexcept {
            if (std::addressof(r) != this) {
                delete[] Raw();
                static_cast<BitsSparseView &>(*this) = static_cast<BitsSparseView &&>(std::move(r));
            }
            return *this;
        }

        ~BitsSparse() noexcept { delete[] Raw(); }
    };
}
