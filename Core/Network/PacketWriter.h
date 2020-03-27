#pragma once

#include "Packet.h"
#include "VarIntHelper.h"
#include <string>
#include <vector>

namespace Network {
    class PacketWriter {
    public:
        explicit PacketWriter(Packet& packet) noexcept
                :mHead(packet.Data()), mPacket(packet) { }
        void UByte(uint8_t v) noexcept { *mHead++ = v; }
        void Bool(bool v) noexcept { UByte(v); }
        void UShort(uint16_t v) noexcept { (UByte(v >> 8u), UByte(v)); }
        void UInt(uint32_t v) noexcept { (UShort(v >> 16u), UShort(v)); }
        void ULong(uint64_t v) noexcept { (UInt(v >> 32u), UInt(v)); }
        // Completed By type punning, which is definitely not allowed by the standard
        void Byte(uint8_t v) noexcept { *mHead++ = v; }
        void Short(int16_t v) noexcept { UShort(v); }
        void Int(int32_t v) noexcept { UShort(v); }
        void Long(int64_t v) noexcept { UShort(v); }
        // The following part is only designed to work on x64
        void Float(float v) noexcept {
            static_assert(std::numeric_limits<float>::is_iec559);
            UInt(*reinterpret_cast<const uint32_t*>(&v)); // the float has to have the same endian as int32
        }
        void Double(double v) noexcept {
            static_assert(std::numeric_limits<double>::is_iec559);
            ULong(*reinterpret_cast<const uint64_t*>(&v)); // the float has to have the same endian as int32
        }
        void VarInt(int v) noexcept { VarIntHelper::WriteAdv(mHead, v); }

        template <class A>
        void String(const std::basic_string<char, std::char_traits<char>, A>& string) noexcept {
            VarInt(string.size());
            for (const auto x: string) Byte(x);
        }

        template <class C>
        void ByteArray(const C& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Byte(x);
        }
        template <class C>
        void UByteArray(const C& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) UByte(x);
        }

        template <class C>
        void ShortArray(const C& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Short(x);
        }
        template <class C>
        void UShortArray(const C& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) UShort(x);
        }

        template <class C>
        void IntArray(const C& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Int(x);
        }
        template <class C>
        void UIntArray(const C& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) UInt(x);
        }

        template <class C>
        void LongArray(const C& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Long(x);
        }
        template <class C>
        void ULongArray(const C& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) ULong(x);
        }

        template <class C>
        void FloatArray(const C& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Float(x);
        }
        template <class C>
        void DoubleArray(const C& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Double(x);
        }

        template <class C>
        void ByteUnboundedArray(const C& array) noexcept {
            for (const auto x : array) Byte(x);
        }
        template <class C>
        void UByteUnboundedArray(const C& array) noexcept {
            for (const auto x : array) UByte(x);
        }

        template <class C>
        void ShortUnboundedArray(const C& array) noexcept {
            for (const auto x : array) Short(x);
        }
        template <class C>
        void UShortUnboundedArray(const C& array) noexcept {
            for (const auto x : array) UShort(x);
        }

        template <class C>
        void IntUnboundedArray(const C& array) noexcept {
            for (const auto x : array) Int(x);
        }
        template <class C>
        void UIntUnboundedArray(const C& array) noexcept {
            for (const auto x : array) UInt(x);
        }

        template <class C>
        void LongUnboundedArray(const C& array) noexcept {
            for (const auto x : array) Long(x);
        }
        template <class C>
        void ULongUnboundedArray(const C& array) noexcept {
            for (const auto x : array) ULong(x);
        }

        template <class C>
        void FloatUnboundedArray(const C& array) noexcept {
            for (const auto x : array) Float(x);
        }
        template <class C>
        void DoubleUnboundedArray(const C& array) noexcept {
            for (const auto x : array) Double(x);
        }

        template <class T, class C>
        void Array(const C& array) noexcept {
            VarInt(array.size());
            for (const auto& x : array) x.Serialize(*this);
        }

        template <class T, class C>
        void UnboundedArray(const C& array) noexcept {
            for (const auto& x : array) x.Serialize(*this);
        }

        static int UUIDSize(UUID uuid) { return 0; } // TODO: implement this
        void UUID(const UUID&) {} // TODO: Implement this

        static int VarIntSize(const int v) noexcept { return VarIntHelper::GetSize(v); }

        template <class A>
        static int StringSize(const std::basic_string<char, std::char_traits<char>, A>& string) noexcept {
            return VarIntSize(string.size())+string.size();
        }

        template <class C>
        static int ByteArraySize(const C& array) noexcept { return VarIntSize(array.size())+array.size(); }
        template <class C>
        static int UByteArraySize(const C& array) noexcept { return VarIntSize(array.size())+array.size(); }

        template <class C>
        static int ShortArraySize(const C& array) noexcept { return VarIntSize(array.size())+array.size()*2; }
        template <class C>
        static int UShortArraySize(const C& array) noexcept {
            return VarIntSize(array.size())+array.size()*2;
        }

        template <class C>
        static int IntArraySize(const C& array) noexcept { return VarIntSize(array.size())+array.size()*4; }
        template <class C>
        static int UIntArraySize(const C& array) noexcept { return VarIntSize(array.size())+array.size()*4; }

        template <class C>
        static int LongArraySize(const C& array) noexcept { return VarIntSize(array.size())+array.size()*8; }
        template <class C>
        static int ULongArraySize(const C& array) noexcept { return VarIntSize(array.size())+array.size()*8; }

        template <class C>
        static int FloatArraySize(const C& array) noexcept { return VarIntSize(array.size())+array.size()*4; }
        template <class C>
        static int DoubleArraySize(const C& array) noexcept { return VarIntSize(array.size())+array.size()*8; }

        template <class C>
        static int ByteUnboundedArraySize(const C& array) noexcept { return array.size(); }
        template <class C>
        static int UByteUnboundedArraySize(const C& array) noexcept { return array.size(); }

        template <class C>
        static int ShortUnboundedArraySize(const C& array) noexcept { return array.size()*2; }
        template <class C>
        static int UShortUnboundedArraySize(const C& array) noexcept { return array.size()*2; }

        template <class C>
        static int IntUnboundedArraySize(const C& array) noexcept { return array.size()*4; }
        template <class C>
        static int UIntUnboundedArraySize(const C& array) noexcept { return array.size()*4; }

        template <class C>
        static int LongUnboundedArraySize(const C& array) noexcept { return array.size()*8; }
        template <class C>
        static int ULongUnboundedArraySize(const C& array) noexcept { return array.size()*8; }

        template <class C>
        static int FloatUnboundedArraySize(const C& array) noexcept { return array.size()*4; }
        template <class C>
        static int DoubleUnboundedArraySize(const C& array) noexcept { return array.size()*8; }

        template <class T, class C>
        static int ArraySize(const C& array) noexcept { return VarIntSize(array.size())+array.size(); }

        template <class T, class C>
        static int UnboundedArraySizeSize(const C& array) noexcept { return array.size(); }

    private:
        uint8_t* mHead;
        Packet& mPacket;
    };
}