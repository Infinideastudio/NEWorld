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
#include <vec3.h>
#include <cstring>

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

    T& operator[](size_t index)
    {
        return data[index];
    }

    T operator[](size_t index) const
    {
        return data[index];
    }

    Mat4 operator*(const Mat4& rhs) const
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

    Mat4& operator*=(const Mat4& rhs)
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
    void transpose()
    {
        std::swap(data[1], data[4]);
        std::swap(data[2], data[8]);
        std::swap(data[3], data[12]);
        std::swap(data[6], data[9]);
        std::swap(data[7], data[13]);
        std::swap(data[11], data[14]);
    }

    // Get transposed matrix
    Mat4 getTranspose() const
    {
        Mat4 res;
        res[0] = data[0] , res[1] = data[4] , res[2] = data[8] , res[3] = data[12];
        res[4] = data[1] , res[5] = data[5] , res[6] = data[9] , res[7] = data[13];
        res[8] = data[2] , res[9] = data[6] , res[10] = data[10] , res[11] = data[14];
        res[12] = data[3] , res[13] = data[7] , res[14] = data[11] , res[15] = data[15];
        return res;
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
    static Mat4 rotation(T degrees, Vec3<T> vec)
    {
        Mat4 res;
        vec.normalize();
        T alpha = degrees * T(M_PI) / T(180.0), s = sin(alpha), c = cos(alpha), t = 1.0f - c;
        res[0] = t * vec.x * vec.x + c;
        res[1] = t * vec.x * vec.y + s * vec.z;
        res[2] = t * vec.x * vec.z - s * vec.y;
        res[4] = t * vec.x * vec.y - s * vec.z;
        res[5] = t * vec.y * vec.y + c;
        res[6] = t * vec.y * vec.z + s * vec.x;
        res[8] = t * vec.x * vec.z + s * vec.y;
        res[9] = t * vec.y * vec.z - s * vec.x;
        res[10] = t * vec.z * vec.z + c;
        res[15] = T(1.0);
        return res;
    }

    // Construct a perspective projection matrix
    static Mat4 perspective(T fov, T aspect, T zNear, T zFar)
    {
        Mat4 res;
        T f = T(1.0) / tan(fov * T(M_PI) / T(180.0) / T(2.0));
        T a = zNear - zFar;
        res[0] = f / aspect;
        res[5] = f;
        res[10] = (zFar + zNear) / a;
        res[11] = T(2.0) * zFar * zNear / a;
        res[14] = T(-1.0);
        return res;
    }

    // Construct an orthogonal projection matrix
    static Mat4 ortho(T left, T right, T top, T bottom, T zNear, T zFar)
    {
        T a = right - left;
        T b = top - bottom;
        T c = zFar - zNear;
        Mat4 res;
        res[0] = T(2.0) / a;
        res[3] = -(right + left) / a;
        res[5] = T(2.0) / b;
        res[7] = -(top + bottom) / b;
        res[10] = T(-2.0) / c;
        res[11] = -(zFar + zNear) / c;
        res[15] = T(1.0);
        return res;
    }
};

using Mat4f = Mat4<float>;
// Recommended to use Mat4f instead of Mat4d
// using Mat4d = Mat4<double>;

#endif // !MAT4_H_
