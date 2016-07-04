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

};

using Mat4f = Mat4<float>;
using Mat4d = Mat4<double>;

#endif // !MAT4_H_
