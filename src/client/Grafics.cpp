#include "Grafics.h"
/*
* NEWorld: An free game with similar rules to Minecraft.
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


Color::Color() :r(0.0f), g(0.0f), b(0.0f), a(0.0f) {};

Color::Color(double _r, double _g, double _b, double _a) :
    r(_r), g(_g), b(_b), a(_a) {};

void Color::Set(double _r, double _g, double _b, double _a)
{
    r = _r; g = _g; b = _b; a = _a;
}

void Color::Get(double & _r, double & _g, double & _b, double & _a) const
{
    _r = r; _g = g; _b = b; _a = a;
}

bool Color::operator==(const Color rhs) const
{
    return (r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a);
}

bool Color::operator!=(const Color rhs) const
{
    return (r != rhs.r) || (g != rhs.g) || (b != rhs.b) || (a != rhs.a);
}

Color Color::operator+(const Color rhs) const
{
    return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
}

Color Color::operator-(const Color rhs) const
{
    return Color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
}

Color Color::operator*(const Color rhs) const
{
    return Color(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
}

Color Color::operator+(const double d) const
{
    return Color(r + d, g + d, b + d, a + d);
}

Color Color::operator-(const double d) const
{
    return Color(r - d, g - d, b - d, a - d);
}

Color Color::operator*(const double d) const
{
    return Color(r * d, g * d, b * d, a * d);
}

Vec2::Vec2() :x(0.0f), y(0.0f) {}

Vec2::Vec2(double _x, double _y) : x(_x), y(_y) {}

void Vec2::Set(double _x, double _y)
{
    x = _x, y = _y;
}

void Vec2::Get(double & _x, double & _y) const
{
    _x = x; _y = y;
}

bool Vec2::operator==(const Vec2 v) const
{
    return (x == v.x) && (y == v.y);
}

bool Vec2::operator!=(const Vec2 v) const
{
    return (x != v.x) || (y != v.y);
}

Vec2 Vec2::operator+(const Vec2 v) const
{
    return Vec2(x + v.x, y + v.y);
}

Vec2 Vec2::operator-(const Vec2 v) const
{
    return Vec2(x - v.x, y + v.y);
}

Vec2 Vec2::operator*(const double d) const
{
    return Vec2(x * d, y * d);
}

Rect::Rect() :xmin(0.0f), xmax(0.0f), ymin(0.0f), ymax(0.0f) {};

Rect::Rect(Vec2 p1, Vec2 p2) :xmin(p1.x), xmax(p2.x), ymin(p1.y), ymax(p2.y) {};

Rect::Rect(double _xmin, double _xmax, double _ymin, double _ymax) :xmin(_xmin), xmax(_xmax), ymin(_ymin), ymax(_ymax) {};

void Rect::Set(double _xmin, double _xmax, double _ymin, double _ymax)
{
    xmin = _xmin; xmax = _xmax; ymin = _ymin; ymax = _ymax;
}

void Rect::Get(double& _xmin, double& _xmax, double& _ymin, double& _ymax) const
{
    _xmin = xmin; _xmax = xmax; _ymin = ymin; _ymax = ymax;
}

Image::Image()
{

}

Image::Image(Image& img)
{

}

Image::Image(std::string path)
{

}

Image Image::Sub(Rect pos)
{
    return Image();
}

Image::~Image()
{

}
