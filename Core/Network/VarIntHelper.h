#pragma once

#include "Packet.h"

namespace Network {
    struct VarIntHelper {
        static constexpr int GetSize(const int v) noexcept {
            const auto uv = static_cast<unsigned int>(v);
            return uv<0x7F ? 1 : uv<0x3FFF ? 2 : uv<0x1FFFFF ? 3 : uv<0xFFFFFFF ? 4 : 5;
        }

        static int Load(const std::uint8_t* data) noexcept { return LoadAdv(data); }

        static int LoadAdv(const std::uint8_t* &data) noexcept {
            int result = 0;
            for (unsigned int shl = 0;; shl += 7) {
                const auto tmp = *(data++);
                const auto val = static_cast<uint32_t>(tmp & 0b01111111u);
                result |= (val << shl); // NOLINT
                if (tmp & 0b10000000u) return result;
            }
        }

        static bool TryLoad(const std::uint8_t* data, const std::uint8_t* const bound, int& result) noexcept {
            return TryLoadAdv(data, bound, result);
        }

        static bool TryLoadAdv(const std::uint8_t* &data, const std::uint8_t* const bound, int& result) noexcept {
            for (unsigned int shl = 0; data<bound; shl += 7) {
                const auto tmp = *(data++);
                const auto val = static_cast<uint32_t>(tmp & 0b01111111u);
                result |= (val << shl); // NOLINT
                if (tmp & 0b10000000u) return true;
            }
            return false;
        }

        static void Write(std::uint8_t* data, int val) noexcept { WriteAdv(data, val); }

        static void WriteAdv(std::uint8_t* &data, int val) noexcept {
            auto i = *reinterpret_cast<uint32_t*>(&val);
            do {
                uint8_t temp = (i & 0b01111111u);
                (*data++) = (i >>= 7u) ? (0b10000000u & temp) : temp;
            }
            while (i!=0);
        }

        static bool TryWrite(std::uint8_t* data, const std::uint8_t* bound, int val) noexcept {
            return TryWriteAdv(data, bound, val);
        }

        static bool TryWriteAdv(std::uint8_t* &data, const std::uint8_t* bound, int val) noexcept {
            auto i = *reinterpret_cast<uint32_t*>(&val);
            for (; data<bound;) {
                uint8_t temp = (i & 0b01111111u);
                if (i >>= 7u) (*data++) = 0b10000000u & temp; else return (*data++ = temp, true);
            }
            return false;
        }
    };
}