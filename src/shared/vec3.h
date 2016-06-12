/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VEC3_H_
#define VEC3_H_

#include <algorithm>
#include <boost/operators.hpp>
#ifdef min
#undef min
#undef max
#endif
template<typename T>
class Vec3:
    boost::totally_ordered<Vec3<T>, boost::additive<Vec3<T>>>
{
public:
    T x, y, z;

    Vec3() :x(), y(), z() {}

    Vec3(T x_, T y_, T z_) :x(x_), y(y_), z(z_) {}

    /// Get the square of vector length, notice that the result can overflow. TODO: fixit
    T lengthSqr() const
    {
        return x*x + y*y + z*z;
    }

    /// Get vector length
    double length() const
    {
        return sqrt(double(lengthSqr()));
    }

    /// Get the Euclidean Distance between vectors
    double euclideanDistance(const Vec3& rhs) const
    {
        return (*this - rhs).length();
    }

    /// Get the Chebyshev Distance between vectors
    T chebyshevDistance(const Vec3& rhs) const
    {
        return std::max(std::max(abs(x - rhs.x), abs(y - rhs.y)), abs(z - rhs.z));
    }

    /// Get the Manhattan Distance between vectors
    T manhattanDistance(const Vec3& rhs) const
    {
        return std::abs(x - rhs.x) + std::abs(y - rhs.y) + std::abs(z - rhs.z);
    }

    bool operator< (const Vec3& rhs) const
    {
        if (x != rhs.x)
            return x < rhs.x;
        if (y != rhs.y)
            return y < rhs.y;
        if (z != rhs.z)
            return z < rhs.z;
        return false;
    }

    bool operator== (const Vec3& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    Vec3& operator+= (const Vec3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vec3& operator-= (const Vec3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    void swap(Vec3& rhs)
    {
        swap(x, rhs.x);
        swap(y, rhs.y);
        swap(z, rhs.z);
    }

    template<typename... ArgType, typename Func>
    void for_each(Func func, ArgType&&... args) const
    {
        func(x, std::forward<ArgType>(args)...);
        func(y, std::forward<ArgType>(args)...);
        func(z, std::forward<ArgType>(args)...);
    }

    template<typename... ArgType, typename Func>
    void for_each(Func func, ArgType&&... args)
    {
        func(x, std::forward<ArgType>(args)...);
        func(y, std::forward<ArgType>(args)...);
        func(z, std::forward<ArgType>(args)...);
    }

    template<typename Func>
    Vec3<T> for_each(Func func) const
    {
        return Vec3<T>(func(x), func(y), func(z));
    }

    template<typename Func>
    Vec3<T> for_each(Func func)
    {
        return Vec3<T>(func(x), func(y), func(z));
    }

    template<typename Func>
    static void for_range(const T& begin, const T& end, Func func)
    {
        Vec3<T> tmp;
        for (tmp.x = begin; tmp.x != end; tmp.x++)
            for (tmp.y = begin; tmp.y != end; tmp.y++)
                for (tmp.z = begin; tmp.z != end; tmp.z++)
                    func(tmp);
    }

    template<typename Func>
    static void for_range(const Vec3<T>& begin, const Vec3<T>& end, Func func)
    {
        Vec3<T> tmp;
        for (tmp.x = begin.x; tmp.x != end.x; tmp.x++)
            for (tmp.y = begin.y; tmp.y != end.y; tmp.y++)
                for (tmp.z = begin.z; tmp.z != end.z; tmp.z++)
                    func(tmp);
    }

    template<typename T, T base>
    T encode()
    {
        return (x*base + y)*base + z;
    }

    template<typename T, T base>
    static Vec3<T> decode(T arg)
    {
        T z = arg % base;
        arg /= base;
        return Vec3<T>(arg / base, arg % base, z);
    }

    template<typename U, typename = typename std::enable_if<std::is_convertible<T, U>::value>::type>
    operator Vec3<U>() const
    {
        return Vec3<U>(x, y, z);
    }
};

using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

#endif // !VEC3_H_
