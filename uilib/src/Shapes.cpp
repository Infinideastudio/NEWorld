#include <uilib.h>
namespace UI
{
    namespace Graphics
    {
        namespace Shapes
        {
            using namespace Core;
            void Line::render()
            {
                Color c = _brush->sample(0.0,0.0);
                glColor4f(c.x, c.y, c.z, c.t);
                glBegin(GL_LINE);
                glVertex2i(_p1.x, _p1.y);
                glVertex2i(_p2.x, _p2.y);
                glEnd();
            }

            Line::Line(const Vec2 & p1, Vec2 p2, const std::shared_ptr<Brush> brush) :
                _p1(p1), _p2(p2), _brush(brush)
            {
            }

            void Line::draw(const Vec2 & p1, const Vec2 & p2, const std::shared_ptr<Brush> brush)
            {
                Color c = brush->sample(0.0, 0.0);
                glColor4f(c.x, c.y, c.z, c.t);
                glBegin(GL_LINE);
                glVertex2i(p1.x, p1.y);
                glVertex2i(p2.x, p2.y);
                glEnd();
            }

            void Triangle::render()
            {
                if (_fbrush)
                {
                    Color c0 = _fbrush->sample(0.0, -1.0);
                    Color c1 = _fbrush->sample(-1.0, 1.0);
                    Color c2 = _fbrush->sample(1.0, 1.0);
                    glBegin(GL_TRIANGLES);
                    glColor4f(c0.x, c0.y, c0.z, c0.t);
                    glVertex2i(_p1.x, _p1.y);
                    glColor4f(c1.x, c1.y, c1.z, c1.t);
                    glVertex2i(_p2.x, _p2.y);
                    glColor4f(c2.x, c2.y, c2.z, c2.t);
                    glVertex2i(_p3.x, _p3.y);
                    glEnd();
                }
                if (_bbrush)
                {
                    Color c0 = _bbrush->sample(0.0, -1.0);
                    Color c1 = _bbrush->sample(-1.0, 1.0);
                    Color c2 = _bbrush->sample(1.0, 1.0);
                    glBegin(GL_LINE_LOOP);
                    glColor4f(c0.x, c0.y, c0.z, c0.t);
                    glVertex2i(_p1.x, _p1.y);
                    glColor4f(c1.x, c1.y, c1.z, c1.t);
                    glVertex2i(_p2.x, _p2.y);
                    glColor4f(c2.x, c2.y, c2.z, c2.t);
                    glVertex2i(_p3.x, _p3.y);
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
                    Color c0 = fbrush->sample(0.0, -1.0);
                    Color c1 = fbrush->sample(-1.0, 1.0);
                    Color c2 = fbrush->sample(1.0, 1.0);
                    glBegin(GL_TRIANGLES);
                    glColor4f(c0.x, c0.y, c0.z, c0.t);
                    glVertex2i(p1.x, p1.y);
                    glColor4f(c1.x, c1.y, c1.z, c1.t);
                    glVertex2i(p2.x, p2.y);
                    glColor4f(c2.x, c2.y, c2.z, c2.t);
                    glVertex2i(p3.x, p3.y);
                    glEnd();
                }
                if (bbrush)
                {
                    Color c0 = bbrush->sample(0.0, -1.0);
                    Color c1 = bbrush->sample(-1.0, 1.0);
                    Color c2 = bbrush->sample(1.0, 1.0);
                    glBegin(GL_LINE_LOOP);
                    glColor4f(c0.x, c0.y, c0.z, c0.t);
                    glVertex2i(p1.x, p1.y);
                    glColor4f(c1.x, c1.y, c1.z, c1.t);
                    glVertex2i(p2.x, p2.y);
                    glColor4f(c2.x, c2.y, c2.z, c2.t);
                    glVertex2i(p3.x, p3.y);
                    glEnd();
                }
            }

            void Rectangle::render()
            {
                if (_fbrush)
                {
                    Color c0 = _fbrush->sample(-1.0,-1.0);
                    Color c1 = _fbrush->sample(-1.0, 1.0);
                    Color c2 = _fbrush->sample(1.0, 1.0);
                    Color c3 = _fbrush->sample(-1.0, 1.0);
                    glBegin(GL_QUADS);
                    glColor4f(c0.x, c0.y, c0.z, c0.t);
                    glVertex2i(_rect.xmin, _rect.ymin);
                    glColor4f(c1.x, c1.y, c1.z, c1.t);
                    glVertex2i(_rect.xmax, _rect.ymin);
                    glColor4f(c2.x, c2.y, c2.z, c2.t);
                    glVertex2i(_rect.xmax, _rect.ymax);
                    glColor4f(c3.x, c3.y, c3.z, c3.t);
                    glVertex2i(_rect.xmin, _rect.ymax);
                    glEnd();
                }
                if (_bbrush)
                {
                    Color c0 = _bbrush->sample(-1.0, -1.0);
                    Color c1 = _bbrush->sample(-1.0, 1.0);
                    Color c2 = _bbrush->sample(1.0, 1.0);
                    Color c3 = _bbrush->sample(-1.0, 1.0);
                    glBegin(GL_LINE_LOOP);
                    glColor4f(c0.x, c0.y, c0.z, c0.t);
                    glVertex2i(_rect.xmin, _rect.ymin);
                    glColor4f(c1.x, c1.y, c1.z, c1.t);
                    glVertex2i(_rect.xmax, _rect.ymin);
                    glColor4f(c2.x, c2.y, c2.z, c2.t);
                    glVertex2i(_rect.xmax, _rect.ymax);
                    glColor4f(c3.x, c3.y, c3.z, c3.t);
                    glVertex2i(_rect.xmin, _rect.ymax);
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
                    Color c0 = fbrush->sample(-1.0, -1.0);
                    Color c1 = fbrush->sample(-1.0, 1.0);
                    Color c2 = fbrush->sample(1.0, 1.0);
                    Color c3 = fbrush->sample(-1.0, 1.0);
                    glBegin(GL_QUADS);
                    glColor4f(c0.x, c0.y, c0.z, c0.t);
                    glVertex2i(rect.xmin, rect.ymin);
                    glColor4f(c1.x, c1.y, c1.z, c1.t);
                    glVertex2i(rect.xmax, rect.ymin);
                    glColor4f(c2.x, c2.y, c2.z, c2.t);
                    glVertex2i(rect.xmax, rect.ymax);
                    glColor4f(c3.x, c3.y, c3.z, c3.t);
                    glVertex2i(rect.xmin, rect.ymax);
                    glEnd();
                }
                if (bbrush)
                {
                    Color c0 = bbrush->sample(-1.0, -1.0);
                    Color c1 = bbrush->sample(-1.0, 1.0);
                    Color c2 = bbrush->sample(1.0, 1.0);
                    Color c3 = bbrush->sample(-1.0, 1.0);
                    glBegin(GL_LINE_LOOP);
                    glColor4f(c0.x, c0.y, c0.z, c0.t);
                    glVertex2i(rect.xmin, rect.ymin);
                    glColor4f(c1.x, c1.y, c1.z, c1.t);
                    glVertex2i(rect.xmax, rect.ymin);
                    glColor4f(c2.x, c2.y, c2.z, c2.t);
                    glVertex2i(rect.xmax, rect.ymax);
                    glColor4f(c3.x, c3.y, c3.z, c3.t);
                    glVertex2i(rect.xmin, rect.ymax);
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