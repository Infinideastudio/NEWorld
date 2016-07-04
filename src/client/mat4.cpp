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

#include "mat4.h"

template <typename T>
inline Mat4<T> Mat4<T>::operator* (const Mat4<T>& rhs) const
{
    Mat4 res;
    res[0] = data[0] * rhs[0] + data[1] * rhs[4] + data[2] * rhs[8] + data[3] * rhs[12];
    res[1] = data[0] * rhs[1] + data[1] * rhs[5] + data[2] * rhs[9] + data[3] * rhs[13];
    res[2] = data[0] * rhs[2] + data[1] * rhs[6] + data[2] * rhs[10] + data[3] * rhs[14];
    res[3] = data[0] * rhs[3] + data[1] * rhs[7] + data[2] * rhs[11] + data[3] * rhs[15];
    res[4] = data[4] * rhs[0] + data[5] * rhs[4] + data[6] * rhs[8] + data[7] * rhs[12];
    res[5] = data[4] * rhs[1] + data[5] * rhs[5] + data[6] * rhs[9] + data[7] * rhs[13];
    res[6] = data[4] * rhs[2] + data[5] * rhs[6] + data[6] * rhs[10] + data[7] * rhs[14];
    res[7] = data[4] * rhs[3] + data[5] * rhs[7] + data[6] * rhs[11] + data[7] * rhs[15];
    res[8] = data[8] * rhs[0] + data[9] * rhs[4] + data[10] * rhs[8] + data[11] * rhs[12];
    res[9] = data[8] * rhs[1] + data[9] * rhs[5] + data[10] * rhs[9] + data[11] * rhs[13];
    res[10] = data[8] * rhs[2] + data[9] * rhs[6] + data[10] * rhs[10] + data[11] * rhs[14];
    res[11] = data[8] * rhs[3] + data[9] * rhs[7] + data[10] * rhs[11] + data[11] * rhs[15];
    res[12] = data[12] * rhs[0] + data[13] * rhs[4] + data[14] * rhs[8] + data[15] * rhs[12];
    res[13] = data[12] * rhs[1] + data[13] * rhs[5] + data[14] * rhs[9] + data[15] * rhs[13];
    res[14] = data[12] * rhs[2] + data[13] * rhs[6] + data[14] * rhs[10] + data[15] * rhs[14];
    res[15] = data[12] * rhs[3] + data[13] * rhs[7] + data[14] * rhs[11] + data[15] * rhs[15];
    return res;
}
