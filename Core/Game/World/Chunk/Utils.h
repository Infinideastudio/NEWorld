#pragma once

#include <cmath>
#include <type_traits>
#include <Math/Vector3.h>

namespace Game::World::Chunk {
    static constexpr int ChunkDem = 32;

    static constexpr int ChunkDemBits = 5;

    template <class T, class = std::enable_if<std::is_arithmetic_v<T>>>
    [[nodiscard]] constexpr T getBlockAxis(const T a) noexcept {
        if constexpr (std::is_integral_v<T>) { return a & T(ChunkDem - 1); }
        else { return std::fmod(a, T(ChunkDem)); }
    }

    template <class T, class = std::enable_if<std::is_arithmetic_v<T>>>
    [[nodiscard]] constexpr T getChunkAxis(const T a) noexcept {
        if constexpr (std::is_integral_v<T>) { return a >> T(ChunkDemBits); }
        else { return std::round(a / T(ChunkDem)); }
    }

    template <class T, class = std::enable_if<std::is_arithmetic_v<T>>>
    [[nodiscard]] constexpr Vec3<T> getBlockPos(const Vec3<T>& a) noexcept {
        if constexpr (std::is_integral_v<T>) { return a & T(ChunkDem - 1); }
        else { return Vec3<T>{getBlockAxis(a.X), getBlockAxis(a.Y), getBlockAxis(a.Z)}; }
    }

    template <class T, class = std::enable_if<std::is_arithmetic_v<T>>>
    [[nodiscard]] constexpr Vec3<T> getChunkPos(const Vec3<T>& a) noexcept {
        if constexpr (std::is_integral_v<T>) { return a >> T(ChunkDemBits); }
        else { return Vec3<T>{getChunkAxis(a.X), getChunkAxis(a.Y), getChunkAxis(a.Z)}; }
    }

    template <class T, class = std::enable_if<std::is_integral_v<T>>>
    [[nodiscard]] constexpr int getBlockPosBlockIndex(const Vec3<T>& a) noexcept {
        return a.X << (ChunkDemBits * 2) | a.Y << ChunkDemBits | a.Z;
    }

    template <class T, class = std::enable_if<std::is_integral_v<T>>>
    [[nodiscard]] constexpr int getBlockIndex(const Vec3<T>& a) noexcept {
        return getBlockPosBlockIndex(getBlockPos(a));
    }
}
