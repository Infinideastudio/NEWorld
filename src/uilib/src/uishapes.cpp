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

#include <uishapes.h>
#include <GL/glew.h>

namespace UI
{
    namespace Graphics
    {
        namespace Shapes
        {
            using namespace Core;
            void Line::render()
            {
                _brush->perpare();
                glBegin(GL_LINE);
                _brush->sample(0.0, 0.0, _p1);
                _brush->sample(0.0, 0.0, _p2);
                glEnd();
            }

            Line::Line(const Vec2 & p1, Vec2 p2, const std::shared_ptr<Brush> brush) :
                _p1(p1), _p2(p2), _brush(brush)
            {
            }

            void Line::draw(const Vec2 & p1, const Vec2 & p2, const std::shared_ptr<Brush> brush)
            {
                brush->perpare();
                glBegin(GL_LINE);
                brush->sample(0.0, 0.0, p1);
                brush->sample(0.0, 0.0, p2);
                glEnd();
            }

            void Triangle::render()
            {
                if (_fbrush)
                {
                    _fbrush->perpare();
                    glBegin(GL_TRIANGLES);
                    _fbrush->sample(0.0, -1.0, _p1);
                    _fbrush->sample(-1.0, 1.0 ,_p2);
                    _fbrush->sample(1.0, 1.0, _p3);
                    glEnd();
                }
                if (_bbrush)
                {
                    _bbrush->perpare();
                    glBegin(GL_LINE_LOOP);
                    _bbrush->sample(0.0, -1.0, _p1);
                    _bbrush->sample(-1.0, 1.0, _p2);
                    _bbrush->sample(1.0, 1.0, _p3);
                    glEnd();
                }
            }

            Triangle::Triangle(const Vec2 & p1, const Vec2 & p2, const Vec2 & p3, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush) :
                _p1(p1), _p2(p2), _p3(p3), _fbrush(fbrush), _bbrush(bbrush)
            {
            }

            void Triangle::draw(const Vec2 & p1, const Vec2 & p2, const Vec2 & p3, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush)
            {
                if (fbrush)
                {
                    fbrush->perpare();
                    glBegin(GL_TRIANGLES);
                    fbrush->sample(0.0, -1.0, p1);
                    fbrush->sample(-1.0, 1.0, p2);
                    fbrush->sample(1.0, 1.0, p3);
                    glEnd();
                }
                if (bbrush)
                {
                    bbrush->perpare();
                    glBegin(GL_LINE_LOOP);
                    bbrush->sample(0.0, -1.0, p1);
                    bbrush->sample(-1.0, 1.0, p2);
                    bbrush->sample(1.0, 1.0, p3);
                    glEnd();
                }
            }

            void Rectangle::render()
            {
                if (_fbrush)
                {
                    _fbrush->perpare();
                    glBegin(GL_QUADS);
                    _fbrush->sample(-1.0, -1.0, Vec2(_rect.xmin, _rect.ymin));
                    _fbrush->sample(-1.0, 1.0, Vec2(_rect.xmax, _rect.ymin));
                    _fbrush->sample(1.0, 1.0, Vec2(_rect.xmax, _rect.ymax));
                    _fbrush->sample(-1.0, 1.0, Vec2(_rect.xmin, _rect.ymax));
                    glEnd();
                }
                if (_bbrush)
                {
                    _bbrush->perpare();
                    glBegin(GL_LINE_LOOP);
                    _bbrush->sample(-1.0, -1.0, Vec2(_rect.xmin, _rect.ymin));
                    _bbrush->sample(-1.0, 1.0, Vec2(_rect.xmax, _rect.ymin));
                    _bbrush->sample(1.0, 1.0, Vec2(_rect.xmax, _rect.ymax));
                    _bbrush->sample(-1.0, 1.0, Vec2(_rect.xmin, _rect.ymax));
                    glEnd();
                }
            }

            Rectangle::Rectangle(const Rect & rect, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush)
                :_rect(rect), _fbrush(fbrush), _bbrush(bbrush)
            {
            }

            void Rectangle::draw(const Rect & rect, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush)
            {
                if (fbrush)
                {
                    fbrush->perpare();
                    glBegin(GL_QUADS);
                    fbrush->sample(-1.0, -1.0, Vec2(rect.xmin, rect.ymin));
                    fbrush->sample(-1.0, 1.0, Vec2(rect.xmax, rect.ymin));
                    fbrush->sample(1.0, 1.0, Vec2(rect.xmax, rect.ymax));
                    fbrush->sample(-1.0, 1.0, Vec2(rect.xmin, rect.ymax));
                    glEnd();
                }
                if (bbrush)
                {
                    bbrush->perpare();
                    glBegin(GL_LINE_LOOP);
                    bbrush->sample(-1.0, -1.0, Vec2(rect.xmin, rect.ymin));
                    bbrush->sample(-1.0, 1.0, Vec2(rect.xmax, rect.ymin));
                    bbrush->sample(1.0, 1.0, Vec2(rect.xmax, rect.ymax));
                    bbrush->sample(-1.0, 1.0, Vec2(rect.xmin, rect.ymax));
                    glEnd();
                }
            }

            void Polygon::render()
            {
            }

            Polygon::Polygon(const std::initializer_list<Vec2>& pts, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush)
            {
            }

            void Polygon::draw(const std::initializer_list<Vec2>& pts, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush)
            {
            }

            void Circle::render()
            {
            }

            Circle::Circle(const Vec2 & center, const double radius, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush)
            {
            }

            void Circle::draw(const Vec2 & center, const double radius, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush)
            {
            }

            void Ellipse::render()
            {
            }

            Ellipse::Ellipse(const Vec2 & f1, const Vec2 & f2, const double l, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush)
            {
            }

            void Ellipse::draw(const Vec2 & f1, const Vec2 & f2, const double l, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush)
            {
            }

        }
    }
}