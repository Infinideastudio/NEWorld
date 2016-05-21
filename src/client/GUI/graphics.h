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

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <string>
#include <vector>
#include <memory>

class UIObject
{
public:
    virtual ~UIObject() {}
};

class UIColor :public UIObject
{
private:
    double r, g, b, a;

public:
    UIColor() :
        r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
    UIColor(double _r, double _g, double _b, double _a = 1.0f) :
        r(_r), g(_g), b(_b), a(_a) {}

    void set(double _r, double _g, double _b, double _a = 1.0f)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    void get(double& _r, double& _g, double& _b, double& _a) const
    {
        _r = r;
        _g = g;
        _b = b;
        _a = a;
    }

    bool operator == (const UIColor& rhs) const
    { return (r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a); }
    bool operator != (const UIColor& rhs) const
    { return (r != rhs.r) || (g != rhs.g) || (b != rhs.b) || (a != rhs.a); }
    UIColor operator + (const UIColor& rhs) const
    { return UIColor(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a); }
    UIColor operator - (const UIColor& rhs) const
    { return UIColor(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a); }
    UIColor operator * (const UIColor& rhs) const
    { return UIColor(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a); }
    UIColor operator + (const double d) const
    { return UIColor(r + d, g + d, b + d, a + d); }
    UIColor operator - (const double d) const
    { return UIColor(r - d, g - d, b - d, a - d); }
    UIColor operator * (const double d) const
    { return UIColor(r * d, g * d, b * d, a * d); }

};

class Vec2 :public UIObject
{
private:
    double x, y;

public:
    Vec2() :x(0), y(0) {}
    Vec2(double _x, double _y) : x(_x), y(_y) {}

    void set(double _x, double _y)
    {
        x = _x, y = _y;
    }

    void get(double& _x, double& _y) const
    {
        _x = x;
        _y = y;
    }

    bool operator == (const Vec2& v) const
    { return (x == v.x) && (y == v.y); }
    bool operator != (const Vec2& v) const
    { return (x != v.x) || (y != v.y); }
    Vec2 operator + (const Vec2& v) const
    { return Vec2(x + v.x, y + v.y); }
    Vec2 operator - (const Vec2& v) const
    { return Vec2(x - v.x, y + v.y); }
    Vec2 operator * (const double d) const
    { return Vec2(x * d, y * d); }

};

class Rect :public UIObject
{
private:
    double xmin, xmax, ymin, ymax;

public:
    Rect() :xmin(0.0f), xmax(0.0f), ymin(0.0f), ymax(0.0f) {}
    Rect(double _xmin, double _xmax, double _ymin, double _ymax) :xmin(_xmin), xmax(_xmax), ymin(_ymin), ymax(_ymax) {}
    Rect(Vec2 p1, Vec2 p2)
    {
        p1.get(xmin, xmax);
        p2.get(ymin, ymax);
    }

    void set(double _xmin, double _xmax, double _ymin, double _ymax)
    {
        xmin = _xmin;
        xmax = _xmax;
        ymin = _ymin;
        ymax = _ymax;
    }

    void get(double& _xmin, double& _xmax, double& _ymin, double& _ymax) const
    {
        _xmin = xmin;
        _xmax = xmax;
        _ymin = ymin;
        _ymax = ymax;
    }
};

class Image :public UIObject
{
private:
    std::shared_ptr<UIObject> img;
public:
    Image();
    Image(Image& img);
    explicit Image(std::string path);

    Image Sub(Rect range);

    ~Image();
};

enum BrushDrawMood
{
    Border, Solid
};

class Brush : public UIObject
{
public:
    virtual void DrawLine(Rect Pos) = 0;
    virtual void DrawTriangle(Vec2 P1, Vec2 P2, Vec2 P3) = 0;
    virtual void DrawRect(Rect Pos, BrushDrawMood Mode) = 0;
    virtual void DrawCircle(Vec2 Center, double radius, BrushDrawMood Mode) = 0;
    virtual void DrawEllipse(Rect Pos, BrushDrawMood Mode) = 0;
    virtual void DrawEllipse(Vec2 Focus1, Vec2 Focus2, double length, BrushDrawMood Mode) = 0;

    virtual void DrawPolygon(std::vector<Vec2>& Points, BrushDrawMood Mode) = 0;
    virtual ~Brush() = 0;
};
#endif // !GRAPHICS_H_
