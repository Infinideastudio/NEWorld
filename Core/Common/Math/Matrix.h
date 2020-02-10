// 
// Core: Matrix.h
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

#include "Vector.h"
#include "Common/Debug.h"

#ifndef M_PI
#define M_PI 3.141592653589793
#endif // !M_PI

#ifdef NEXWORLD_DEBUG

// Mat4Row for range checking when debugging is enabled
template <typename T>
class Mat4Row {
public:
    T * ptr;

    explicit Mat4Row(T* p) : ptr(p) {}

    T& operator[](size_t index) {
        Assert(index < 4);
        return ptr[index];
    }
};

#endif

template <typename T>
class Mat4 {
public:
    static constexpr double Pi = 3.1415926535897932;

    T data[16];

    Mat4() { memset(data, 0, sizeof(data)); }
    Mat4(const Mat4& rhs) { memcpy(data, rhs.data, sizeof(data)); }

    explicit Mat4(T x) {
        memset(data, 0, sizeof(data));
        data[0] = data[5] = data[10] = data[15] = x; // Identity matrix
    }

    explicit Mat4(T* src) { memcpy(data, src, sizeof(data)); }

#ifndef NEXWORLD_DEBUG

    T* operator[](size_t index) { return data + index * 4; }

#else

    Mat4Row<T> operator[](size_t index) {
        Assert(index < 4);
        return Mat4Row<T>(data + index * 4);
    }

#endif

    Mat4& operator+=(const Mat4& rhs) {
        for (int i = 0; i < 16; i += 4) {
            data[i + 0] += rhs.data[i + 0];
            data[i + 1] += rhs.data[i + 1];
            data[i + 2] += rhs.data[i + 2];
            data[i + 3] += rhs.data[i + 3];
        }
        return *this;
    }

    Mat4 operator*(const Mat4& rhs) const {
        Mat4 res;
        res.data[0] = data[0] * rhs.data[0] + data[1] * rhs.data[4] + data[2] * rhs.data[8] + data[3] * rhs.data[12];
        res.data[1] = data[0] * rhs.data[1] + data[1] * rhs.data[5] + data[2] * rhs.data[9] + data[3] * rhs.data[13];
        res.data[2] = data[0] * rhs.data[2] + data[1] * rhs.data[6] + data[2] * rhs.data[10] + data[3] * rhs.data[14];
        res.data[3] = data[0] * rhs.data[3] + data[1] * rhs.data[7] + data[2] * rhs.data[11] + data[3] * rhs.data[15];
        res.data[4] = data[4] * rhs.data[0] + data[5] * rhs.data[4] + data[6] * rhs.data[8] + data[7] * rhs.data[12];
        res.data[5] = data[4] * rhs.data[1] + data[5] * rhs.data[5] + data[6] * rhs.data[9] + data[7] * rhs.data[13];
        res.data[6] = data[4] * rhs.data[2] + data[5] * rhs.data[6] + data[6] * rhs.data[10] + data[7] * rhs.data[14];
        res.data[7] = data[4] * rhs.data[3] + data[5] * rhs.data[7] + data[6] * rhs.data[11] + data[7] * rhs.data[15];
        res.data[8] = data[8] * rhs.data[0] + data[9] * rhs.data[4] + data[10] * rhs.data[8] + data[11] * rhs.data[12];
        res.data[9] = data[8] * rhs.data[1] + data[9] * rhs.data[5] + data[10] * rhs.data[9] + data[11] * rhs.data[13];
        res.data[10] = data[8] * rhs.data[2] + data[9] * rhs.data[6] + data[10] * rhs.data[10] + data[11] * rhs.data[14
        ];
        res.data[11] = data[8] * rhs.data[3] + data[9] * rhs.data[7] + data[10] * rhs.data[11] + data[11] * rhs.data[15
        ];
        res.data[12] = data[12] * rhs.data[0] + data[13] * rhs.data[4] + data[14] * rhs.data[8] + data[15] * rhs.data[12
        ];
        res.data[13] = data[12] * rhs.data[1] + data[13] * rhs.data[5] + data[14] * rhs.data[9] + data[15] * rhs.data[13
        ];
        res.data[14] = data[12] * rhs.data[2] + data[13] * rhs.data[6] + data[14] * rhs.data[10] + data[15] * rhs.data[
            14];
        res.data[15] = data[12] * rhs.data[3] + data[13] * rhs.data[7] + data[14] * rhs.data[11] + data[15] * rhs.data[
            15];
        return res;
    }

    Mat4& operator*=(const Mat4& rhs) {
        *this = *this * rhs;
        return *this;
    }

    // Swap row r1, row r2
    void swapRows(size_t r1, size_t r2) {
        Assert(r1 < 4 && r2 < 4);
        std::swap(data[r1 * 4 + 0], data[r2 * 4 + 0]);
        std::swap(data[r1 * 4 + 1], data[r2 * 4 + 1]);
        std::swap(data[r1 * 4 + 2], data[r2 * 4 + 2]);
        std::swap(data[r1 * 4 + 3], data[r2 * 4 + 3]);
    }

    // Row r *= k
    void multRow(size_t r, T k) {
        Assert(r < 4);
        data[r * 4 + 0] *= k;
        data[r * 4 + 1] *= k;
        data[r * 4 + 2] *= k;
        data[r * 4 + 3] *= k;
    }

    // Row dst += row src * k
    void multAndAdd(size_t src, size_t dst, T k) {
        Assert(dst < 4 && src < 4);
        data[dst * 4 + 0] += data[src * 4 + 0] * k;
        data[dst * 4 + 1] += data[src * 4 + 1] * k;
        data[dst * 4 + 2] += data[src * 4 + 2] * k;
        data[dst * 4 + 3] += data[src * 4 + 3] * k;
    }

    // Transpose matrix
    void transpose() {
        std::swap(data[1], data[4]);
        std::swap(data[2], data[8]);
        std::swap(data[3], data[12]);
        std::swap(data[6], data[9]);
        std::swap(data[7], data[13]);
        std::swap(data[11], data[14]);
    }

    // Get transposed matrix
    Mat4 getTranspose() const {
        Mat4 res;
        res.data[0] = data[0], res.data[1] = data[4], res.data[2] = data[8], res.data[3] = data[12];
        res.data[4] = data[1], res.data[5] = data[5], res.data[6] = data[9], res.data[7] = data[13];
        res.data[8] = data[2], res.data[9] = data[6], res.data[10] = data[10], res.data[11] = data[14];
        res.data[12] = data[3], res.data[13] = data[7], res.data[14] = data[11], res.data[15] = data[15];
        return res;
    }

    // Inverse matrix
    Mat4& inverse() {
        Mat4 res(T(1));
        for (int i = 0; i < 4; i++) {
            int p = i;
            for (int j = i + 1; j < 4; j++) { if (abs(data[j * 4 + i]) > abs(data[p * 4 + i])) p = j; }
            res.swapRows(i, p);
            swapRows(i, p);
            res.multRow(i, T(1) / data[i * 4 + i]);
            multRow(i, T(1) / data[i * 4 + i]);
            for (int j = i + 1; j < 4; j++) {
                res.multAndAdd(i, j, -data[j * 4 + i]);
                multAndAdd(i, j, -data[j * 4 + i]);
            }
        }
        for (int i = 3; i >= 0; i--) {
            for (int j = 0; j < i; j++) {
                res.multAndAdd(i, j, -data[j * 4 + i]);
                multAndAdd(i, j, -data[j * 4 + i]);
            }
        }
        (*this) = res;
        return *this;
    }

    // Construct a translation matrix
    static Mat4 translation(const Vec3<T>& delta) {
        Mat4 res(T(1.0));
        res.data[3] = delta.x;
        res.data[7] = delta.y;
        res.data[11] = delta.z;
        return res;
    }

    // Construct a rotation matrix
    static Mat4 rotation(T degrees, Vec3<T> vec) {
        Mat4 res;
        vec.normalize();
        T alpha = degrees * T(Pi) / T(180.0), s = sin(alpha), c = cos(alpha), t = 1.0f - c;
        res.data[0] = t * vec.x * vec.x + c;
        res.data[1] = t * vec.x * vec.y - s * vec.z;
        res.data[2] = t * vec.x * vec.z + s * vec.y;
        res.data[4] = t * vec.x * vec.y + s * vec.z;
        res.data[5] = t * vec.y * vec.y + c;
        res.data[6] = t * vec.y * vec.z - s * vec.x;
        res.data[8] = t * vec.x * vec.z - s * vec.y;
        res.data[9] = t * vec.y * vec.z + s * vec.x;
        res.data[10] = t * vec.z * vec.z + c;
        res.data[15] = T(1.0);
        return res;
    }

    // Construct a perspective projection matrix
    static Mat4 perspective(T fov, T aspect, T zNear, T zFar) {
        Mat4 res;
        T f = T(1) / tan(fov * T(Pi) / T(180) / T(2));
        T a = zNear - zFar;
        res.data[0] = f / aspect;
        res.data[5] = f;
        res.data[10] = (zFar + zNear) / a;
        res.data[11] = T(2) * zFar * zNear / a;
        res.data[14] = T(-1);
        return res;
    }

    // Construct an orthogonal projection matrix
    static Mat4 ortho(T left, T right, T top, T bottom, T zNear, T zFar) {
        T a = right - left;
        T b = top - bottom;
        T c = zFar - zNear;
        Mat4 res;
        res.data[0] = T(2) / a;
        res.data[3] = -(right + left) / a;
        res.data[5] = T(2) / b;
        res.data[7] = -(top + bottom) / b;
        res.data[10] = T(-2) / c;
        res.data[11] = -(zFar + zNear) / c;
        res.data[15] = T(1);
        return res;
    }

    // Multiply with Vec4(vec, w)
    std::pair<Vec3<T>, T> transform(const Vec3<T>& vec, T w) const {
        Vec3<T> res(data[0] * vec.x + data[1] * vec.y + data[2] * vec.z + data[3] * w,
                    data[4] * vec.x + data[5] * vec.y + data[6] * vec.z + data[7] * w,
                    data[8] * vec.x + data[9] * vec.y + data[10] * vec.z + data[11] * w);
        T rw = data[12] * vec.x + data[13] * vec.y + data[14] * vec.z + data[15] * w;
        return std::make_pair(res, rw);
    }
};

typedef Mat4<float> Mat4f;
// It is recommended to use Mat4f instead of Mat4d if you are dealing with rendering.
typedef Mat4<float> Mat4d;
