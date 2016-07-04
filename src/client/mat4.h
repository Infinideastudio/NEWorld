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
    explicit Mat4(const T& x)
    {
        memset(data, 0, sizeof(data));
        data[0] = data[5] = data[10] = data[15] = x; // Identity matrix
    }

    T& operator[] (int index)
    {
        return data[index];
    }
    Mat4 operator* (const Mat4& rhs) const;
    Mat4& operator*= (const Mat4& rhs)
    {
        *this = *this * rhs;
        return this;
    }

    // Get data by position
    int get(int x, int y)
    {
        return data[y * 4 + x];
    }

    // Set data by position
    void set(int x, int y, const T& v)
    {
        data[y * 4 + x] = v;
    }

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
    static Mat4 rotation(const T& degrees, const Vec3<T>& scale);
    // Construct a perspective projection matrix
    static Mat4 perspective(const T& fov, const T& aspect, const T& zNear, const T& zFar)
    {
        Mat4 res;
        float viewAngleH = fov * T(M_PI) / T(180.0);
        float viewAngleV = atan(tan(viewAngleH / T(2.0)) * aspect) * T(2.0);
        res[0] = T(1.0) / tan(viewAngleV / T(2.0));
        res[5] = res[0] * aspect;
        res[10] = -(zFar + zNear) / (zFar - zNear);
        res[11] = T(-1.0);
        res[14] = T(-2.0) * zFar * zNear / (zFar - zNear);
        return res;
    }
    // Construct an orthogonal projection matrix
    static Mat4 ortho(const T& left, const T& right, const T& top, const T& bottom, const T& zNear, const T& zFar)
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
using Mat4d = Mat4<double>;

#endif // !MAT4_H_
