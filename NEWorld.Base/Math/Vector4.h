#pragma once

#include <cmath>
#include <algorithm>
#include <type_traits>

template<class T, class = std::enable_if<std::is_arithmetic_v<T>>>
struct Vec4 {
    union {
        T Data[4];
        struct {
            T X, Y, Z, W;
        };
    };

    constexpr Vec4() noexcept = default;

    constexpr Vec4(T x, T y, T z, T w) noexcept
            : X(x), Y(y), Z(z), W(w) {}

    constexpr explicit Vec4(T v) noexcept
            : X(v), Y(v), Z(v), W(v) {}

    template<typename U, class = std::enable_if_t<std::is_convertible_v<T, U>>>
    constexpr Vec4(const Vec4<U> &r) noexcept // NOLINT
            : X(T(r.X)), Y(T(r.Y)), Z(T(r.Z)), W(T(r.W)) {}

    template<typename U, class F>
    constexpr Vec4(const Vec4<U> &r, F transform) noexcept // NOLINT
            : X(transform(r.X)), Y(transform(r.Y)), Z(transform(r.Z)), W(transform(r.W)) {}


    constexpr Vec4 operator+(const Vec4 &r) const noexcept { return {X + r.X, Y + r.Y, Z + r.Z, W + r.W}; }

    constexpr Vec4 operator-(const Vec4 &r) const noexcept { return {X - r.X, Y - r.Y, Z - r.Z, W - r.W}; }

    template<class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec4 operator*(U r) const noexcept { return {X * r, Y * r, Z * r, W * r}; }

    template<class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec4 operator/(U r) const noexcept { return {X / r, Y / r, Z / r, W / r}; }

    constexpr Vec4 &operator+=(const Vec4 &r) noexcept { return (X += r.X, Y += r.Y, Z += r.Z, W += r.W, *this); }

    constexpr Vec4 &operator-=(const Vec4 &r) noexcept { return (X -= r.X, Y -= r.Y, Z -= r.Z, W -= r.W, *this); }

    template<class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec4 &operator*=(U r) noexcept { return (X *= r, Y *= r, Z *= r, W *= r, *this); }

    template<class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec4 &operator/=(U r) noexcept { return (X /= r, Y /= r, Z /= r, W /= r, *this); }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 operator<<(T r) const noexcept { return {X << r, Y << r, Z << r, W << r}; }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 operator<<(const Vec4 &r) const noexcept { return {X << r.X, Y << r.Y, Z << r.Z, W << r.W}; }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 &operator<<=(T r) noexcept { return (X <<= r, Y <<= r, Z <<= r, W <<= r, *this); }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 &operator<<=(const Vec4 &r) noexcept { return (X <<= r.X, Y <<= r.Y, Z <<= r.Z, W <<= r.W, *this); }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 operator>>(T r) const noexcept { return {X >> r, Y >> r, Z >> r, W >> r}; }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 operator>>(const Vec4 &r) const noexcept { return {X >> r.X, Y >> r.Y, Z >> r.Z, W >> r.W}; }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 &operator>>=(T r) noexcept { return (X >>= r, Y >>= r, Z >>= r, W >>= r, *this); }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 &operator>>=(const Vec4 &r) noexcept { return (X >>= r.X, Y >>= r.Y, Z >>= r.Z, W >>= r.W, *this); }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 operator|(T r) const noexcept { return {X | r, Y | r, Z | r, W | r}; }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 operator|(const Vec4 &r) const noexcept { return {X | r.X, Y | r.Y, Z | r.Z, W | r.W}; }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 &operator|=(T r) noexcept { return (X |= r, Y |= r, Z |= r, W |= r, *this); }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 &operator|=(const Vec4 &r) noexcept { return (X |= r.X, Y |= r.Y, Z |= r.Z, W |= r.W, *this); }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 operator&(T r) const noexcept { return {X & r, Y & r, Z & r, W & r}; }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 operator&(const Vec4 &r) const noexcept { return {X & r.X, Y & r.Y, Z & r.Z, W & r.W}; }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 &operator&=(T r) noexcept { return (X &= r, Y &= r, Z &= r, W &= r, *this); }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 &operator&=(const Vec4 &r) noexcept { return (X &= r.X, Y &= r.Y, Z &= r.Z, W &= r.W, *this); }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 operator^(T r) const noexcept { return {X ^ r, Y ^ r, Z ^ r, W ^ r}; }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 operator^(const Vec4 &r) const noexcept { return {X ^ r.X, Y ^ r.Y, Z ^ r.Z, W ^ r.W}; }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 &operator^=(T r) noexcept { return (X ^= r, Y ^= r, Z ^= r, W ^= r.W, *this); }

    template<class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec4 &operator^=(const Vec4 &r) noexcept { return (X ^= r.X, Y ^= r.Y, Z ^= r.Z, W ^= r.W, *this); }

    constexpr Vec4 operator-() const noexcept { return {-X, -Y, -Z, -W}; }

    [[nodiscard]] constexpr T LengthSquared() const noexcept { return X * X + Y * Y + Z * Z + W * W; }

    constexpr bool operator==(const Vec4 &r) const noexcept {
        return (X == r.X) && (Y == r.Y) && (Z == r.Z) && (W == r.W);
    }

    constexpr bool operator<(const Vec4 &r) const noexcept { return LengthSquared() < r.LengthSquared(); }

    constexpr bool operator>(const Vec4 &r) const noexcept { return LengthSquared() > r.LengthSquared(); }

    constexpr bool operator<=(const Vec4 &r) const noexcept { return LengthSquared() <= r.LengthSquared(); }

    constexpr bool operator>=(const Vec4 &r) const noexcept { return LengthSquared() >= r.LengthSquared(); }

    [[nodiscard]] constexpr T Dot(const Vec4 &r) const noexcept { return X * r.X + Y * r.Y + Z * r.Z + W * r.W; }

    [[nodiscard]] T Length() const noexcept { return std::sqrt(LengthSquared()); }

    void Normalize() noexcept { (*this) /= Length(); }
};

template<class T>
constexpr T Dot(const Vec4<T> &l, const Vec4<T> &r) noexcept { return l.Dot(r); }

template<class T, class U, class = std::enable_if<std::is_arithmetic_v<U>>>
constexpr Vec4<T> operator*(U l, const Vec4<T> &r) noexcept { return r * l; }

template<class T>
double EuclideanDistanceSquared(const Vec4<T> &l, const Vec4<T> &r) noexcept {
    return (l - r).LengthSquared();
}

template<class T>
double EuclideanDistance(const Vec4<T> &l, const Vec4<T> &r) noexcept {
    return (l - r).Length();
}

template<class T>
double DistanceSquared(const Vec4<T> &l, const Vec4<T> &r) noexcept {
    return (l - r).LengthSquared();
}

template<class T>
double Distance(const Vec4<T> &l, const Vec4<T> &r) noexcept {
    return (l - r).Length();
}

template<class T>
constexpr T ChebyshevDistance(const Vec4<T> &l, const Vec4<T> &r) noexcept {
    return std::max(std::max(std::max(std::abs(l.X - r.X), std::abs(l.Y - r.Y)),std::abs(l.Z - r.Z)), std::abs(l.W - r.W));
}

template<class T>
constexpr T ManhattanDistance(const Vec4<T> &l, const Vec4<T> &r) noexcept {
    return std::abs(l.X - r.X) + std::abs(l.Y - r.Y) + std::abs(l.Z - r.Z) + std::abs(l.W - r.W);
}

template<class T, class F>
void Cursor(const Vec4<T> &min, const Vec4<T> &max, F func) noexcept {
    Vec4<T> vec{};
    for (vec.X = min.X; vec.X < max.X; ++vec.X)
        for (vec.Y = min.Y; vec.Y < max.Y; ++vec.Y)
            for (vec.Z = min.Z; vec.Z < max.Z; ++vec.Z)
                for (vec.W = min.W; vec.W < max.W; ++vec.W)
                    func(vec);
}

using Char4 = Vec4<int8_t>;
using Byte4 = Vec4<uint8_t>;
using Short4 = Vec4<int16_t>;
using UShort4 = Vec4<uint16_t>;
using Int4 = Vec4<int32_t>;
using UInt4 = Vec4<uint32_t>;
using Long4 = Vec4<int64_t>;
using ULong4 = Vec4<uint64_t>;
using Float4 = Vec4<float>;
using Double4 = Vec4<double>;
