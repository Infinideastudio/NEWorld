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

    explicit Mat4(T* src)
    {
        memcpy(data, src, sizeof(data));
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
    int get(size_t x, size_t y) const
    {
        return data[y * 4 + x];
    }

    // Set data by position
    void set(size_t x, size_t y, T v)
    {
        data[y * 4 + x] = v;
    }

    // Transpose matrix
    void transpose();

    // Get transposed matrix
    Mat4 getTranspose() const;

    // Construct a translation matrix
    static Mat4 translation(const Vec3<T>& delta)
    {
        Mat4 res(T(1.0));
        res[3] = delta.x;
        res[7] = delta.y;
        res[11] = delta.z;
        return res;
    }

    // Construct a rotation matrix
    static Mat4 rotation(T degrees, Vec3<T> vec);

    // Construct a perspective projection matrix
    static Mat4 perspective(T fov, T aspect, T zNear, T zFar);

    // Construct an orthogonal projection matrix
    static Mat4 ortho(T left, T right, T top, T bottom, T zNear, T zFar)
    {
        Mat4 res;
        res[0] = T(2.0) / (right - left);
        res[5] = T(2.0) / (bottom - top);
        res[10] = T(2.0) / (zNear - zFar);
        res[15] = T(1.0);
        return res;
    }

};

using Mat4f = Mat4<float>;
// Recommended to use Mat4f instead of Mat4d
// using Mat4d = Mat4<double>;

#endif // !MAT4_H_
