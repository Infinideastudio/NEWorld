#pragma once

#include <cstdint>
#include "Math/Vector3.h"
#include "Game/Network/States/Base.h"
#include "Game/Network/PacketReader.h"

namespace Game::Network::Protocol {
    using Bool = bool;
    using Byte = std::int8_t;
    using Short = std::int16_t;
    using Int = std::int32_t;
    using Long = std::int64_t;
    using UByte = std::uint8_t;
    using UShort = std::uint16_t;
    using UInt = std::uint32_t;
    using ULong = std::uint64_t;
    using Float = float;
    using Double = double;
    using VarInt = Int;
    using String = std::basic_string<char, std::char_traits<char>, Temp::Allocator<char>>;
    using ByteArray = std::vector<int8_t, Temp::Allocator<int8_t>>;
    using UByteArray = std::vector<uint8_t, Temp::Allocator<uint8_t>>;
    using ShortArray = std::vector<int16_t, Temp::Allocator<int16_t>>;
    using UShortArray = std::vector<uint16_t, Temp::Allocator<uint16_t>>;
    using IntArray = std::vector<int32_t, Temp::Allocator<int32_t>>;
    using UIntArray = std::vector<uint32_t, Temp::Allocator<uint32_t>>;
    using LongArray = std::vector<int64_t, Temp::Allocator<int64_t>>;
    using ULongArray = std::vector<uint64_t, Temp::Allocator<uint64_t>>;
    using VarIntArray = std::vector<int32_t, Temp::Allocator<int32_t>>;
    using FloatArray = std::vector<float, Temp::Allocator<float>>;
    using DoubleArray = std::vector<double, Temp::Allocator<double>>;
    using ByteUnboundedArray = ByteArray;
    using UByteUnboundedArray = UByteArray;
    using ShortUnboundedArray = ShortArray;
    using UShortUnboundedArray = UShortArray;
    using IntUnboundedArray = IntArray;
    using UIntUnboundedArray = UIntArray;
    using LongUnboundedArray = LongArray;
    using ULongUnboundedArray = ULongArray;
    using VarIntUnboundedArray = VarIntArray;
    using FloatUnboundedArray = FloatArray;
    using DoubleUnboundedArray = DoubleArray;

    inline constexpr Int3 DecodePosition(uint64_t l) noexcept {
        int x = int64_t(l) >> 38; // NOLINT
        int y = l & 0xfff; // NOLINT
        int z = (int64_t(l) << 26 >> 38); // NOLINT
        return {x,y,z};
    }
}