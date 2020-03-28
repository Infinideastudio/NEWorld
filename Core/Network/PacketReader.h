#pragma once

#include "Packet.h"
#include "VarIntHelper.h"
#include <string>
#include <vector>
#include <Cfx/Utilities/TempAlloc.h>

namespace Network {
    class PacketReader {
    public:
        explicit PacketReader(Packet&& packet) noexcept
                :mHead(packet.Data()), mPacket(std::move(packet)) { }

        uint8_t UByte() noexcept { return *(mHead++); }

        bool Bool() noexcept { return UByte(); }

        uint16_t UShort() noexcept {
            auto hi = UByte();
            auto lo = UByte();
            return uint16_t(hi) << 8u | lo;
        }
        uint32_t UInt() noexcept {
            auto hi = UShort();
            auto lo = UShort();
            return uint32_t(hi) << 16u | lo;
        }
        uint64_t ULong() noexcept {
            auto hi = UInt();
            auto lo = UInt();
            return uint64_t(hi) << 32u | lo;
        }
        // Completed By type punning, which is definitely not allowed by the standard
        int8_t Byte() noexcept { return *reinterpret_cast<const int8_t*>(mHead++); } // NOLINT
        int16_t Short() noexcept {
            auto hi = Byte();
            auto lo = Byte();
            return int16_t(hi) << 8u | lo;
        }
        int32_t Int() noexcept {
            auto hi = Short();
            auto lo = Short();
            return int32_t(hi) << 16u | lo;
        }
        int64_t Long() noexcept {
            auto hi = Int();
            auto lo = Int();
            return int64_t(hi) << 32u | lo;
        }
        // The following part is only designed to work on x64
        float Float() noexcept {
            static_assert(std::numeric_limits<float>::is_iec559);
            const auto u32 = UInt();
            return *reinterpret_cast<const float*>(&u32); // the float has to have the same endian as int32
        }
        double Double() noexcept {
            static_assert(std::numeric_limits<double>::is_iec559);
            const auto u64 = UInt();
            return *reinterpret_cast<const double*>(&u64); // the double has to have the same endian as int64
        }
        int VarInt() noexcept { return VarIntHelper::LoadAdv(mHead); }

        template <class A = Temp::Allocator<char>>
        std::basic_string<char, std::char_traits<char>, A> String() {
            const auto size = VarInt();
            if (Remains()>=size*sizeof(char)) {
                std::basic_string<char, std::char_traits<char>, A> ret(size, '\0');
                for (auto& x : ret) { x = Byte(); }
                return ret;
            }
            InternalFail();
            return {};
        }

        template <class A = Temp::Allocator<int8_t>>
        std::vector<int8_t, A> ByteArray() noexcept {
            const auto size = VarInt();
            if (Remains()>=size*sizeof(int8_t)) {
                std::vector<int8_t, A> ret(size);
                for (auto& x : ret) { x = Byte(); }
                return ret;
            }
            InternalFail();
            return {};
        }
        template <class A = Temp::Allocator<uint8_t>>
        std::vector<uint8_t, A> UByteArray() noexcept {
            const auto size = VarInt();
            if (Remains()>=size*sizeof(uint8_t)) {
                std::vector<uint8_t, A> ret(size);
                for (auto& x : ret) { x = UByte(); }
                return ret;
            }
            InternalFail();
            return {};
        }

        template <class A = Temp::Allocator<int16_t>>
        std::vector<int16_t, A> ShortArray() noexcept {
            const auto size = VarInt();
            if (Remains()>=size*sizeof(int16_t)) {
                std::vector<int16_t, A> ret(size);
                for (auto& x : ret) { x = Short(); }
                return ret;
            }
            InternalFail();
            return {};
        }
        template <class A = Temp::Allocator<uint16_t>>
        std::vector<uint16_t, A> UShortArray() noexcept {
            const auto size = VarInt();
            if (Remains()>=size*sizeof(uint16_t)) {
                std::vector<uint16_t, A> ret(size);
                for (auto& x : ret) { x = UShort(); }
                return ret;
            }
            InternalFail();
            return {};
        }

        template <class A = Temp::Allocator<int32_t>>
        std::vector<int32_t, A> IntArray() noexcept {
            const auto size = VarInt();
            if (Remains()>=size*sizeof(int32_t)) {
                std::vector<int32_t, A> ret(size);
                for (auto& x : ret) { x = Int(); }
                return ret;
            }
            InternalFail();
            return {};
        }
        template <class A = Temp::Allocator<uint32_t>>
        std::vector<uint32_t, A> UIntArray() noexcept {
            const auto size = VarInt();
            if (Remains()>=size*sizeof(uint32_t)) {
                std::vector<uint32_t, A> ret(size);
                for (auto& x : ret) { x = UInt(); }
                return ret;
            }
            InternalFail();
            return {};
        }

        template <class A = Temp::Allocator<int64_t>>
        std::vector<int64_t, A> LongArray() noexcept {
            const auto size = VarInt();
            if (Remains()>=size*sizeof(int64_t)) {
                std::vector<int64_t, A> ret(size);
                for (auto& x : ret) { x = Long(); }
                return ret;
            }
            InternalFail();
            return {};
        }
        template <class A = Temp::Allocator<uint64_t>>
        std::vector<uint64_t, A> ULongArray() noexcept {
            const auto size = VarInt();
            if (Remains()>=size*sizeof(uint64_t)) {
                std::vector<uint64_t, A> ret(size);
                for (auto& x : ret) { x = ULong(); }
                return ret;
            }
            InternalFail();
            return {};
        }

        template <class A = Temp::Allocator<float>>
        std::vector<float, A> FloatArray() noexcept {
            const auto size = VarInt();
            if (Remains()>=size*sizeof(float)) {
                std::vector<float, A> ret(size);
                for (auto& x : ret) { x = Float(); }
                return ret;
            }
            InternalFail();
            return {};
        }
        template <class A = Temp::Allocator<double>>
        std::vector<double, A> DoubleArray() noexcept {
            const auto size = VarInt();
            if (Remains()>=size*sizeof(double)) {
                std::vector<double, A> ret(size);
                for (auto& x : ret) { x = Double(); }
                return ret;
            }
            InternalFail();
            return {};
        }

        template <class A = Temp::Allocator<int8_t>>
        std::vector<int8_t, A> ByteUnboundedArray() noexcept {
            if (Good()) {
                const auto size = Remains();
                std::vector<int8_t, A> ret(size);
                for (auto& x : ret) { x = Byte(); }
                return ret;
            }
            InternalFail();
            return {};
        }
        template <class A = Temp::Allocator<uint8_t>>
        std::vector<uint8_t, A> UByteUnboundedArray() noexcept {
            if (Good()) {
                const auto size = Remains();
                std::vector<uint8_t, A> ret(size);
                for (auto& x : ret) { x = UByte(); }
                return ret;
            }
            InternalFail();
            return {};
        }

        template <class A = Temp::Allocator<int16_t>>
        std::vector<int16_t, A> ShortUnboundedArray() noexcept {
            if (Good()) {
                const auto size = Remains()/2;
                if (size*2!=Remains()) {
                    std::vector<int16_t, A> ret(size);
                    for (auto& x : ret) { x = Short(); }
                    return ret;
                }
            }
            InternalFail();
            return {};
        }
        template <class A = Temp::Allocator<uint16_t>>
        std::vector<uint16_t, A> UShortUnboundedArray() noexcept {
            if (Good()) {
                const auto size = Remains()/2;
                if (size*2!=Remains()) {
                    std::vector<uint16_t, A> ret(size);
                    for (auto& x : ret) { x = UShort(); }
                    return ret;
                }
            }
            InternalFail();
            return {};
        }

        template <class A = Temp::Allocator<int32_t>>
        std::vector<int32_t, A> IntUnboundedArray() noexcept {
            if (Good()) {
                const auto size = Remains()/4;
                if (size*4!=Remains()) {
                    std::vector<int32_t, A> ret(size);
                    for (auto& x : ret) { x = Int(); }
                    return ret;
                }
            }
            InternalFail();
            return {};
        }
        template <class A = Temp::Allocator<uint32_t>>
        std::vector<uint32_t, A> UIntUnboundedArray() noexcept {
            if (Good()) {
                const auto size = Remains()/4;
                if (size*4!=Remains()) {
                    std::vector<uint32_t, A> ret(size);
                    for (auto& x : ret) { x = UInt(); }
                    return ret;
                }
            }
            InternalFail();
            return {};
        }

        template <class A = Temp::Allocator<int64_t>>
        std::vector<int64_t, A> LongUnboundedArray() noexcept {
            if (Good()) {
                const auto size = Remains()/8;
                if (size*8!=Remains()) {
                    std::vector<int64_t, A> ret(size);
                    for (auto& x : ret) { x = Long(); }
                    return ret;
                }
            }
            InternalFail();
            return {};
        }
        template <class A = Temp::Allocator<uint64_t>>
        std::vector<uint64_t, A> ULongUnboundedArray() noexcept {
            if (Good()) {
                const auto size = Remains()/8;
                if (size*8!=Remains()) {
                    std::vector<uint64_t, A> ret(size);
                    for (auto& x : ret) { x = ULong(); }
                    return ret;
                }
            }
            InternalFail();
            return {};
        }

        template <class A = Temp::Allocator<float>>
        std::vector<float, A> FloatUnboundedArray() noexcept {
            if (Good()) {
                const auto size = Remains()/4;
                if (size*4!=Remains()) {
                    std::vector<float, A> ret(size);
                    for (auto& x : ret) { x = Float(); }
                    return ret;
                }
            }
            InternalFail();
            return {};
        }
        template <class A = Temp::Allocator<double>>
        std::vector<double, A> DoubleUnboundedArray() noexcept {
            if (Good()) {
                const auto size = VarInt();
                if (Remains()>=size*sizeof(double)) {
                    std::vector<double, A> ret(size);
                    for (auto& x : ret) { x = Double(); }
                    return ret;
                }
            }
            InternalFail();
            return {};
        }

        [[nodiscard]] long Remains() const noexcept { return static_cast<long>(mPacket.Data()+mPacket.Size()-mHead); }

        [[nodiscard]] bool Good() const noexcept { return Remains()>=0; }
    private:
        void InternalFail() noexcept { mHead = mPacket.Data()+mPacket.Size()+1; }
        const uint8_t* mHead;
        Packet mPacket;
    };
}
