#pragma once

#include <cstdint>
#include <algorithm>
#include <Cfx/Utilities/TempAlloc.h>

namespace Network {
    class Packet {
    public:
        constexpr Packet() noexcept
                :mSize(), mSA(), mPayload(nullptr) { }
        explicit Packet(int size) noexcept
                :mSize(size), mSA(size), mPayload(static_cast<uint8_t*>(Temp::Allocate(size))) { }
        Packet(int size, std::uint8_t* load) noexcept
                :mSize(size), mSA(size), mPayload(load) { }
        Packet(const Packet&) = delete;
        Packet& operator=(const Packet&) = delete;
        Packet(Packet&& r) noexcept
                :mSize(r.mSize), mSA(r.mSA), mPayload(r.mPayload) { r.mPayload = nullptr; }
        Packet(Packet&& r, int size) noexcept
                :mSize(size), mSA(r.mSA), mPayload(r.mPayload) { r.mPayload = nullptr; }
        Packet& operator=(Packet&& r) noexcept {
            if (std::addressof(r)!=this) { // reuse dtor of moved target
                (std::swap(mSize, r.mSize), std::swap(mPayload, r.mPayload));
            }
            return *this;
        }
        ~Packet() noexcept { if (mPayload) { Temp::Deallocate(mPayload, mSA); }}

        [[nodiscard]] auto Size() const noexcept { return mSize; }
        [[nodiscard]] auto Data() noexcept { return mPayload; }
        [[nodiscard]] auto Data() const noexcept { return static_cast<const std::uint8_t*>(mPayload); }
    private:
        int mSize, mSA;
        std::uint8_t* mPayload;
    };
}
