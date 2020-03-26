#pragma once

#include "Packet.h"
#include "VarInt.h"
#include <string>
#include <vector>

namespace Game::Network {
    class PacketWriter {
    public:
        void UByte(const uint8_t v) noexcept { *mHead++ = v; }
        void UShort(const uint16_t v) noexcept { (UByte(v >> 8u), UByte(v)); }
        void UInt(const uint32_t v) noexcept { (UShort(v >> 16u), UShort(v)); }
        void ULong(const uint64_t v) noexcept { (UInt(v >> 32u), UInt(v)); }
        // Completed By type punning, which is definitely not allowed by the standard
        void Byte(const uint8_t v) noexcept { *mHead++ = v; }
        void Short(const int16_t v) noexcept { UShort(reinterpret_cast<int16_t>(v)); }
        void Int(const int32_t v) noexcept { UShort(reinterpret_cast<int32_t>(v)); }
        void Long(const int64_t v) noexcept { UShort(reinterpret_cast<int64_t>(v)); }
        // The following part is only designed to work on x64
        void Float(const float v) noexcept {
            static_assert(std::numeric_limits<float>::is_iec559);
            UInt(*reinterpret_cast<const uint32_t*>(&v)); // the float has to have the same endian as int32
        }
        void Double(const double v) noexcept {
            static_assert(std::numeric_limits<double>::is_iec559);
            ULong(*reinterpret_cast<const uint64_t*>(&v)); // the float has to have the same endian as int32
        }
        void VarInt(const int v) noexcept { VarInt::WriteAdv(mHead, v); }

        template<class A>
        void String(const std::basic_string<char, std::char_traits<char>, A>& string) noexcept {
            VarInt(string.size());
            for (const auto x: string) Byte(x);
        }

        template<template<class> class C>
        void ByteArray(const C<int8_t>& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Byte(x);
        }
        template<template<class> class C>
        void UByteArray(const C<uint8_t>& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) UByte(x);
        }

        template<template<class> class C>
        void ShortArray(const C<int16_t>& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Short(x);
        }
        template<template<class> class C>
        void UShortArray(const C<uint16_t>& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) UShort(x);
        }

        template<template<class> class C>
        void IntArray(const C<int32_t>& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Int(x);
        }
        template<template<class> class C>
        void UIntArray(const C<uint32_t>& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) UInt(x);
        }

        template<template<class> class C>
        void LongArray(const C<int64_t>& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Long(x);
        }
        template<template<class> class C>
        void ULongArray(const C<uint64_t>& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) ULong(x);
        }

        template<template<class> class C>
        void FloatArray(const C<float>& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Float(x);
        }
        template<template<class> class C>
        void DoubleArray(const C<double>& array) noexcept {
            VarInt(array.size());
            for (const auto x : array) Double(x);
        }

        template<template<class> class C>
        void ByteUnboundedArray(const C<int8_t>& array) noexcept {
            for (const auto x : array) Byte(x);
        }
        template<template<class> class C>
        void UByteUnboundedArray(const C<uint8_t>& array) noexcept {
            for (const auto x : array) UByte(x);
        }

        template<template<class> class C>
        void ShortUnboundedArray(const C<int16_t>& array) noexcept {
            for (const auto x : array) Short(x);
        }
        template<template<class> class C>
        void UShortUnboundedArray(const C<uint16_t>& array) noexcept {
            for (const auto x : array) UShort(x);
        }

        template<template<class> class C>
        void IntUnboundedArray(const C<int32_t>& array) noexcept {
            for (const auto x : array) Int(x);
        }
        template<template<class> class C>
        void UIntUnboundedArray(const C<uint32_t>& array) noexcept {
            for (const auto x : array) UInt(x);
        }

        template<template<class> class C>
        void LongUnboundedArray(const C<int64_t>& array) noexcept {
            for (const auto x : array) Long(x);
        }
        template<template<class> class C>
        void ULongUnboundedArray(const C<uint64_t>& array) noexcept {
            for (const auto x : array) ULong(x);
        }

        template<template<class> class C>
        void FloatUnboundedArray(const C<float>& array) noexcept {
            for (const auto x : array) Float(x);
        }
        template<template<class> class C>
        void DoubleUnboundedArray(const C<double>& array) noexcept {
            for (const auto x : array) Double(x);
        }

        template<class T, template<class> class C>
        void Array(const C<T>& array) noexcept {
            VarInt(array.size());
            for (const auto& x : array) x.Serialize(*this);
        }

        template<class T, template<class> class C>
        void UnboundedArray(const C<T>& array) noexcept {
            for (const auto& x : array) x.Serialize(*this);
        }
    private:
        uint8_t* mHead;
        Packet mPacket;
    };
}