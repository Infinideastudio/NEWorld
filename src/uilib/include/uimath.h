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

#ifndef UIMATH_H_
#define UIMATH_H_

#include "uiplatform.h"

namespace UI
{
    namespace Math
    {
        class UILIB_API Vec2
        {
        public:
            double x, y;
            Vec2() = default;
            Vec2(double _x, double _y);
            Vec2(const Vec2& from, const Vec2& to);
            bool operator == (const Vec2& v) const;
            bool operator != (const Vec2& v) const;
            Vec2 operator + (const Vec2& v) const;
            Vec2 operator - (const Vec2& v) const;
            Vec2 operator * (const double d) const;
            Vec2& set(const Vec2& to);
            Vec2& set(const double _x, const double _y);
        };

        class UILIB_API Vec3
        {
        public:
            double x, y, z;
            Vec3() = default;
            Vec3(double _x, double _y, double _z);
            Vec3(const Vec3& from, const Vec3& to);
            Vec3 operator - (const Vec3& b) const;
            Vec3 operator + (const Vec3& b) const;
            Vec3 operator * (const double b) const;
            Vec3 operator * (const Vec3& b) const; //Cross Product
            Vec3& operator -= (const Vec3& b);
            Vec3& operator += (const Vec3& b);
            Vec3& operator *= (const double b);
            Vec3& operator *= (const Vec3& b); //Cross Product
            Vec3& zero();
            Vec3& set(const Vec3& to);
            Vec3& set(const double _x, const double _y, const double _z);
            double length() const;
            Vec3 unit() const;
            double dot(const Vec3& b) const;
        };

        class UILIB_API Vec4
        {
        public:
            double x, y, z, t;
            Vec4() = default;
            Vec4(double _x, double _y, double _z, double _t);
            Vec4(const Vec4& from, const Vec4& to);
            Vec4 operator - (const Vec4& b) const;
            Vec4 operator + (const Vec4& b) const;
            Vec4 operator * (const double b) const;
            Vec4& operator -= (const Vec4& b);
            Vec4& operator += (const Vec4& b);
            Vec4& operator *= (const double b);
            Vec4& zero();
            Vec4& set(const Vec4& to);
            Vec4& set(const double _x, const double _y, const double _z, const double _t);
            double length() const;
            Vec4 unit() const;
            double dot(const Vec4& b) const;
        };
    }
}
#endif
