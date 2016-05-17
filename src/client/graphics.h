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

class Object
{
public:
    virtual ~Object() = 0;
};

class Color :public Object
{
public:
    double r, g, b, a;
    Color();
    Color(double _r, double _g, double _b, double _a = 1.0f);

    void Set(double _r, double _g, double _b, double _a = 1.0f);
    void Get(double& _r, double& _g, double& _b, double& _a) const;

    bool operator == (const Color rhs) const;
    bool operator != (const Color rhs) const;

    Color operator + (const Color rhs) const;
    Color operator - (const Color rhs) const;
    Color operator * (const Color rhs) const;

    Color operator + (const double d) const;
    Color operator - (const double d) const;
    Color operator * (const double d) const;
};

class Vec2 :public Object
{
public:
    double x, y;
    Vec2();
    Vec2(double _x, double _y);

    void Set(double _x, double _y);
    void Get(double& _x, double& _y) const;

    bool operator == (const Vec2 v) const;
    bool operator != (const Vec2 v) const;

    Vec2 operator + (const Vec2 v) const;
    Vec2 operator - (const Vec2 v) const;

    Vec2 operator * (const double d) const;
};

class Rect :public Object
{
public:
    double xmin, xmax, ymin, ymax;
    Rect();
    Rect(Vec2 p1, Vec2 p2);
    Rect(double _xmin, double _xmax, double _ymin, double _ymax);

    void Set(double _xmin, double _xmax, double _ymin, double _ymax);
    void Get(double& _xmin, double& _xmax, double& _ymin, double& _ymax) const;
};

class Image :public Object
{
private:
    std::shared_ptr<Object> hdc;
public:
    Image();
    Image(Image& img);
    Image(std::string path);

    Image Sub(Rect pos);

    ~Image();
};

enum BrushDrawMood
{
    Border, Solid
};

class Brush : public Object
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
