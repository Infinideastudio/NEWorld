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

#ifndef MAT4_H_
#define MAT4_H_

#include <cmath>
#include <cstring> // memset, memcpy
#include <vec3.h>

template <typename T>
class Mat4
{
public:
    T data[16];

    Mat4()
    {
        memset(data, 0, sizeof(data));
    }

    Mat4(const Mat4& rhs)
    {
        memcpy(data, rhs.data, sizeof(data));
    }

    explicit Mat4(T x)
    {
        memset(data, 0, sizeof(data));
        data[0] = data[5] = data[10] = data[15] = x; // Identity matrix
    }

    T& operator[] (size_t index)
    {
        return data[index];
    }

    T operator[] (size_t index) const
    {
        return data[index];
    }

    Mat4 operator* (const Mat4& rhs) const;

    Mat4& operator*= (const Mat4& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    // Get data by position
    int get(size_t x, size_t y)
    {
        return data[y * 4 + x];
    }

    // Set data by position
    void set(size_t x, size_t y, T v)
    {
        data[y * 4 + x] = v;
    }
};

using Mat4f = Mat4<float>;
using Mat4d = Mat4<double>;

// Construct a translation matrix
template <typename T>
inline Mat4<T> translation(const Vec3<T>& delta)
{
    Mat4<T> res(T(1.0));
    res[3] = delta.x;
    res[7] = delta.y;
    res[11] = delta.z;
    return res;
}

// Construct a perspective projection matrix
template <typename T>
inline Mat4<T> perspective(T fov, T aspect, T zNear, T zFar)
{
    Mat4<T> res;
    float viewAngleH = fov * T(M_PI) / T(180.0), viewAngleV = atan(tan(viewAngleH / T(2.0)) * aspect) * T(2.0);
    res[0] = T(1.0) / tan(viewAngleV / T(2.0));
    res[5] = res[0] * aspect;
    res[10] = -(zFar + zNear) / (zFar - zNear);
    res[11] = T(-1.0);
    res[14] = T(-2.0) * zFar * zNear / (zFar - zNear);
    return res;
}

// Construct an orthogonal projection matrix
template <typename T>
inline Mat4<T> ortho(T left, T right, T top, T bottom, T zNear, T zFar)
{
    Mat4<T> res;
    res[0] = T(2.0) / (right - left);
    res[5] = T(2.0) / (bottom - top);
    res[10] = T(2.0) / (zNear - zFar);
    res[15] = T(1.0);
    return res;
}

// Construct a rotation matrix
template <typename T>
inline Mat4<T> rotation(T degrees, const Vec3<T>& scale)
{
    Mat4<T> res;
    T length = sqrt(scale.lengthSqr());
    x /= length;
    y /= length;
    z /= length;
    T alpha = degrees * T(M_PI) / T(180.0), s = sin(alpha), c = cos(alpha), t = 1.0f - c;
    res[0] = t * scale.x * scale.x + c;
    res[1] = t * scale.x * scale.y + s * scale.z;
    res[2] = t * scale.x * scale.z - s * scale.y;
    res[4] = t * scale.x * scale.y - s * scale.z;
    res[5] = t * scale.y * scale.y + c;
    res[6] = t * scale.y * scale.z + s * scale.x;
    res[8] = t * scale.x * scale.z + s * scale.y;
    res[9] = t * scale.y * scale.z - s * scale.x;
    res[10] = t * scale.z * scale.z + c;
    res[15] = T(1.0);
    return res;
}

#endif // !MAT4_H_
