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

#include <cstring> // memset, memcpy

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

    Mat4 operator* (const Mat4& rhs) const;

    // Get data by position
    static int get(int x, int y)
    {
        return data[y*4 + x];
    }

    // Set data by position
    void et(int x, int y, const T& v)
    {
        data[y * 4 + x] = v;
    }

};

using Mat4f = Mat4<float>;
using Mat4d = Mat4<double>;

#endif // !MAT4_H_
