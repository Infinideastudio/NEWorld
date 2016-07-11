/*
UILib - A Open-Source UI-Library

Copyright(C) 2016 Infinideastudio-UITeam

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and / or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#include <uimath.h>
#include <math.h>

namespace UI
{
    namespace Math
    {
        Vec2::Vec2(double _x, double _y) : x(_x), y(_y) 
        {
        }

        Vec2::Vec2(const Vec2 & from, const Vec2 & to) :
            x(to.x - from.x), y(to.y - from.y)
        {
        }

        bool Vec2::operator == (const Vec2& v) const
        {
            return (x == v.x) && (y == v.y);
        }

        bool Vec2::operator != (const Vec2& v) const
        {
            return (x != v.x) || (y != v.y);
        }

        Vec2 Vec2::operator + (const Vec2& v) const
        {
            return Vec2(x + v.x, y + v.y);
        }

        Vec2 Vec2::operator - (const Vec2& v) const
        {
            return Vec2(x - v.x, y + v.y);
        }

        Vec2 Vec2::operator * (const double d) const
        {
            return Vec2(x * d, y * d);
        }

        Vec2 & Vec2::set(const Vec2 & to)
        {
            x = to.x;
            y = to.y;
            return *this;
        }

        Vec2 & Vec2::set(const double _x, const double _y)
        {
            x = _x;
            y = _y;
            return *this;
        }

        Vec3::Vec3(double _x, double _y, double _z) :
            x(_x), y(_y), z(_z)
        {
        }

        Vec3::Vec3(const Vec3 & from, const Vec3 & to)
            : x(to.x - from.x), y(to.y - from.y), z(to.z - from.z)
        {
        }

        Vec3 Vec3::operator-(const Vec3 & b) const
        {
            return Vec3(x - b.x, y - b.y, z - b.z);
        }

        Vec3 Vec3::operator+(const Vec3 & b) const
        {
            return Vec3(x + b.x, y + b.y, z + b.z);
        }

        Vec3 Vec3::operator*(const double b) const
        {
            return Vec3(x * b, y * b, z * b);
        }

        Vec3 Vec3::operator*(const Vec3 & b) const
        {
            return Vec3(y * b.z - z * b.y, z * b.x - x * b.z,
                x * b.y - y * b.x);
        }

        Vec3& Vec3::operator-=(const Vec3 & b)
        {
            x -= b.x; y -= b.y; z -= b.z;
            return *this;
        }

        Vec3& Vec3::operator+=(const Vec3 & b)
        {
            x += b.x; y += b.y; z += b.z;
            return *this;
        }

        Vec3& Vec3::operator*=(const double b)
        {
            x *= b, y *= b, z *= b;
            return *this;
        }

        Vec3& Vec3::operator*=(const Vec3 & b)
        {
            x = y * b.z - z * b.y;
            y = z * b.x - x * b.z;
            z = x * b.y - y * b.x;
            return *this;
        }

        Vec3 & Vec3::zero()
        {
            x = y = z = 0;
            return *this;
        }

        Vec3 & Vec3::set(const Vec3 & to)
        {
            x = to.x;
            y = to.y;
            z = to.z;
            return *this;
        }

        Vec3 & Vec3::set(const double _x, const double _y, const double _z)
        {
            x = _x;
            y = _y;
            z = _z;
            return *this;
        }

        double Vec3::length() const
        {
            return (sqrt(x * x + y * y + z * z));
        }

        Vec3 Vec3::unit() const
        {
            const double l = length();
            return Vec3(x / l, y / l, z / l);
        }

        double Vec3::dot(const Vec3 & b) const
        {
            return(x * b.x + y * b.y + z * b.z);
        }

        Vec4::Vec4(double _x, double _y, double _z, double _t) :
            x(_x), y(_y), z(_z), t(_t)
        {
        }

        Vec4::Vec4(const Vec4 & from, const Vec4 & to) :
            x(to.x - from.x), y(to.y - from.y), z(to.z - from.z), t(to.t - from.t)
        {
        }

        Vec4 Vec4::operator-(const Vec4 & b) const
        {
            return Vec4(x - b.x, y - b.y, z - b.z, t - b.t);
        }

        Vec4 Vec4::operator+(const Vec4 & b) const
        {
            return Vec4(x + b.x, y + b.y, z + b.z, t + b.t);
        }

        Vec4 Vec4::operator*(const double b) const
        {
            return Vec4(x * b, y * b, z * b, t * b);
        }

        Vec4& Vec4::operator-=(const Vec4 & b)
        {
            x -= b.x; y -= b.y; z -= b.z; t -= b.t;
            return *this;
        }

        Vec4& Vec4::operator+=(const Vec4 & b)
        {
            x += b.x; y += b.y; z += b.z; t += b.t;
            return *this;
        }

        Vec4& Vec4::operator*=(const double b)
        {
            x *= b, y *= b, z *= b; y *= b;
            return *this;
        }

        Vec4 & Vec4::zero()
        {
            x = y = z = t = 0;
            return *this;
        }

        Vec4 & Vec4::set(const Vec4 & to)
        {
            x = to.x;
            y = to.y;
            z = to.z;
            t = to.t;
            return *this;
        }

        Vec4 & Vec4::set(const double _x, const double _y, const double _z, const double _t)
        {
            x = _x;
            y = _y;
            z = _z;
            t = _t;
            return *this;
        }

        double Vec4::length() const
        {
            return (sqrt(x * x + y * y + z * z + t * t));
        }

        Vec4 Vec4::unit() const
        {
            const double l = length();
            return Vec4(x / l, y / l, z / l, t/l);
        }

        double Vec4::dot(const Vec4 & b) const
        {
            return(x * b.x + y * b.y + z * b.z + t * b.t);
        }
    }
}
