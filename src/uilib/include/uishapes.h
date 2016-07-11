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

#ifndef UISHAPES_H_
#define UISHAPES_H_

#include "uicore.h"

namespace UI
{
    namespace Graphics
    {
        using namespace Math;
        using namespace Base;
        namespace Shapes
        {
            class UILIB_API Line : public Core::Control
            {
            public:
                Vec2 _p1, _p2;
                std::shared_ptr<Brush> _brush;
                void render();
                Line(const Vec2& p1, Vec2 p2, const std::shared_ptr<Brush> brush);
                static void draw(const Vec2& p1, const Vec2& p2, const std::shared_ptr<Brush> brush);
            };

            class UILIB_API Triangle : public Core::Control
            {
            public:
                Vec2 _p1, _p2, _p3;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Triangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Vec2& p1, const Vec2& p2, const Vec2& p3, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Rectangle : public Core::Control
            {
            public:
                Rect _rect;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Rectangle(const Rect& rect, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Rect& rect, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Polygon : public Core::Control
            {
            public:
                std::vector<Vec2> _pts;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Polygon(const std::initializer_list<Vec2>& pts, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const std::initializer_list<Vec2>& pts, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Circle : public Core::Control
            {
            public:
                Vec2 _center;
                double _radius;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Circle(const Vec2& center, const double radius, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Vec2& center, const double radius, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Ellipse : public Core::Control
            {
            public:
                Vec2 _f1, _f2;
                double l;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Ellipse(const Vec2& f1, const Vec2& f2, const double l, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Vec2& f1, const Vec2& f2, const double l, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };
        }
    }
}

#endif
