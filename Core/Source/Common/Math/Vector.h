// 
// Core: Vector.h
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#pragma once

#include <cmath>
#include <type_traits>
#include <utility>
#include <functional>
#include <rpc/msgpack/adaptor/define_decl.hpp>

template <typename T>
class Vec2 {
public:
    using param_type = T;

    T x, y;

    MSGPACK_DEFINE_ARRAY(x, y)

    constexpr Vec2() : x(), y() { }

    constexpr Vec2(param_type x_, param_type y_) : x(x_), y(y_) { }

    constexpr Vec2(param_type value) : x(value), y(value) { }

    template <typename U, std::enable_if_t<std::is_convertible<T, U>::value, int>  = 0>
    constexpr Vec2(const Vec2<U>& rhs) : x(T(rhs.x)), y(T(rhs.y)) { }

    // Get the square of vector lengths
    T lengthSqr() const noexcept { return x * x + y * y; }

    // Get vector length
    double length() const noexcept { return sqrt(double(lengthSqr())); }

    // Get the Euclidean Distance between vectors
    double euclideanDistance(const Vec2& rhs) const noexcept { return (*this - rhs).length(); }

    // Get the Chebyshev Distance between vectors
    T chebyshevDistance(const Vec2& rhs) const noexcept { return max(max(abs(x - rhs.x), abs(y - rhs.y))); }

    // Get the Manhattan Distance between vectors
    T manhattanDistance(const Vec2& rhs) const noexcept { return abs(x - rhs.x) + abs(y - rhs.y); }

    // Normalize vector
    void normalize() noexcept {
        double l = length();
        x /= l;
        y /= l;
    }

    bool operator<(const Vec2& rhs) const noexcept {
        if (x != rhs.x)
            return x < rhs.x;
        if (y != rhs.y)
            return y < rhs.y;
        return false;
    }

    bool operator==(const Vec2& rhs) const noexcept { return x == rhs.x && y == rhs.y; }

    Vec2& operator+=(const Vec2& rhs) noexcept {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& rhs) noexcept {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vec2<T>& operator*=(T value) noexcept {
        x *= value;
        y *= value;
        return *this;
    }

    Vec2<T>& operator/=(T value) noexcept {
        x /= value;
        y /= value;
        return *this;
    }

    Vec2<T> operator*(T value) const noexcept { return Vec2<T>(x * value, y * value); }

    Vec2<T> operator/(T value) const noexcept { return Vec2<T>(x / value, y / value); }

    bool operator!=(const Vec2& rhs) const noexcept { return !(rhs == *this); }

    const Vec2<T> operator+(const Vec2<T>& rhs) const noexcept {
        Vec2<T> tmp(*this);
        tmp += rhs;
        return tmp;
    };

    const Vec2<T> operator-(const Vec2<T>& rhs) const noexcept {
        Vec2<T> tmp(*this);
        tmp -= rhs;
        return tmp;
    };

    const Vec2<T> operator*(const Vec2<T>& rhs) const noexcept {
        Vec2<T> tmp(*this);
        tmp *= rhs;
        return tmp;
    };

    const Vec2<T> operator/(const Vec2<T>& rhs) const noexcept {
        Vec2<T> tmp(*this);
        tmp /= rhs;
        return tmp;
    };

    void swap(Vec2& rhs) noexcept {
        std::swap(x, rhs.x);
        std::swap(y, rhs.y);
    }

    friend Vec2<T> operator-(const Vec2<T>& vec) noexcept { return Vec2<T>(-vec.x, -vec.y); }

    template <class Vec2Type>
    Vec2Type conv() const noexcept { return Vec2Type(x, y); }

};

template <typename T>
class Vec3 {
public:
    using param_type = T;

    T x, y, z;

    MSGPACK_DEFINE_ARRAY(x, y, z)

    constexpr Vec3() : x(), y(), z() { }

    constexpr Vec3(param_type x_, param_type y_, param_type z_) : x(x_), y(y_), z(z_) { }

    constexpr Vec3(param_type value) : x(value), y(value), z(value) { }

    template <typename U, std::enable_if_t<std::is_convertible<T, U>::value, int>  = 0>
    constexpr Vec3(const Vec3<U>& rhs) : x(T(rhs.x)), y(T(rhs.y)), z(T(rhs.z)) { }

    // Get the square of vector length
    T lengthSqr() const noexcept { return x * x + y * y + z * z; }

    // Get vector length
    double length() const noexcept { return sqrt(double(lengthSqr())); }

    // Get the Euclidean Distance between vectors
    double euclideanDistance(const Vec3& rhs) const noexcept { return (*this - rhs).length(); }

    // Get the Chebyshev Distance between vectors
    T chebyshevDistance(const Vec3& rhs) const noexcept {
        return max(max(abs(x - rhs.x), abs(y - rhs.y)), abs(z - rhs.z));
    }

    // Get the Manhattan Distance between vectors
    T manhattanDistance(const Vec3& rhs) const noexcept { return abs(x - rhs.x) + abs(y - rhs.y) + abs(z - rhs.z); }

    // Normalize vector
    Vec3 normalize() { return (*this) / length(); }

    bool operator<(const Vec3& rhs) const noexcept {
        if (x != rhs.x)
            return x < rhs.x;
        if (y != rhs.y)
            return y < rhs.y;
        if (z != rhs.z)
            return z < rhs.z;
        return false;
    }

    bool operator==(const Vec3& rhs) const noexcept { return x == rhs.x && y == rhs.y && z == rhs.z; }

    Vec3& operator+=(const Vec3& rhs) noexcept {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& rhs) noexcept {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    Vec3<T>& operator*=(T value) noexcept {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }

    Vec3<T>& operator/=(T value) noexcept {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }

    Vec3<T> operator*(T value) const noexcept { return Vec3<T>(x * value, y * value, z * value); }

    Vec3<T> operator/(T value) const noexcept { return Vec3<T>(x / value, y / value, z / value); }

    bool operator!=(const Vec3& rhs) const noexcept { return !(rhs == *this); }

    const Vec3<T> operator+(const Vec3<T>& rhs) const noexcept {
        Vec3<T> tmp(*this);
        tmp += rhs;
        return tmp;
    };

    const Vec3<T> operator-(const Vec3<T>& rhs) const noexcept {
        Vec3<T> tmp(*this);
        tmp -= rhs;
        return tmp;
    };

    const Vec3<T> operator*(const Vec3<T>& rhs) const noexcept {
        Vec3<T> tmp(*this);
        tmp *= rhs;
        return tmp;
    };

    const Vec3<T> operator/(const Vec3<T>& rhs) const noexcept {
        Vec3<T> tmp(*this);
        tmp /= rhs;
        return tmp;
    };

    void swap(Vec3& rhs) noexcept {
        std::swap(x, rhs.x);
        std::swap(y, rhs.y);
        std::swap(z, rhs.z);
    }

    template <typename... ArgType, typename Func>
    void for_each(Func func, ArgType&&... args) const {
        func(x, std::forward<ArgType>(args)...);
        func(y, std::forward<ArgType>(args)...);
        func(z, std::forward<ArgType>(args)...);
    }

    template <typename... ArgType, typename Func>
    void for_each(Func func, ArgType&&... args) {
        func(x, std::forward<ArgType>(args)...);
        func(y, std::forward<ArgType>(args)...);
        func(z, std::forward<ArgType>(args)...);
    }

    //TODO: fix it. And tell if "for_each" will change the value of "this".
    template <typename Func>
    Vec3<T> transform(Func func) const { return Vec3<T>(func(x), func(y), func(z)); }

    template <typename Func>
    Vec3<T> transform(Func func) { return Vec3<T>(func(x), func(y), func(z)); }

    template <typename Func>
    static void for_range(T begin, T end, Func func) {
        Vec3<T> tmp;
        for (tmp.x = begin; tmp.x < end; ++tmp.x)
            for (tmp.y = begin; tmp.y < end; ++tmp.y)
                for (tmp.z = begin; tmp.z < end; ++tmp.z)
                    func(tmp);
    }

    template <typename Func>
    static void for_range(const Vec3<T>& begin, const Vec3<T>& end, Func func) {
        Vec3<T> tmp;
        for (tmp.x = begin.x; tmp.x < end.x; ++tmp.x)
            for (tmp.y = begin.y; tmp.y < end.y; ++tmp.y)
                for (tmp.z = begin.z; tmp.z < end.z; ++tmp.z)
                    func(tmp);
    }

    template <typename U, U base>
    U encode() const noexcept { return (x * base + y) * base + z; }

    template <typename U, U base>
    static Vec3<U> decode(T arg) noexcept {
        U z = arg % base;
        arg /= base;
        return Vec3<U>(arg / base, arg % base, z);
    }

    friend Vec3<T> operator-(const Vec3<T>& vec) noexcept { return Vec3<T>(-vec.x, -vec.y, -vec.z); }

    template <class Vec3Type>
    Vec3Type conv() const noexcept { return Vec3Type(x, y, z); }

private:
    // to solve problems about `abs`, we need this.
    static T abs(param_type arg) noexcept { return arg >= 0 ? arg : -arg; }

    static T max(param_type arg1, param_type arg2) noexcept { return arg1 > arg2 ? arg1 : arg2; }
};

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

namespace std {
    template <>
    struct hash<Vec3i> {
        using argument_type = Vec3i;
        using result_type = std::size_t;

        result_type operator()(argument_type const& s) const noexcept {
            size_t x = s.x;
            x = (x) & 0xFFFF00000000FFFF;
            x = (x | (x << 16)) & 0x00FF0000FF0000FF;
            x = (x | (x << 8)) & 0xF00F00F00F00F00F;
            x = (x | (x << 4)) & 0x30C30C30C30C30C3;
            x = (x | (x << 2)) & 0x9249249249249249;

            size_t y = s.y;
            y = (y) & 0xFFFF00000000FFFF;
            y = (y | (y << 16)) & 0x00FF0000FF0000FF;
            y = (y | (y << 8)) & 0xF00F00F00F00F00F;
            y = (y | (y << 4)) & 0x30C30C30C30C30C3;
            y = (y | (y << 2)) & 0x9249249249249249;

            size_t z = s.z;
            z = (z) & 0xFFFF00000000FFFF;
            z = (z | (z << 16)) & 0x00FF0000FF0000FF;
            z = (z | (z << 8)) & 0xF00F00F00F00F00F;
            z = (z | (z << 4)) & 0x30C30C30C30C30C3;
            z = (z | (z << 2)) & 0x9249249249249249;
            return x | (y << 1) | (z << 2);
        }
    };
}
