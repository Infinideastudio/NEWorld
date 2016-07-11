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

#include <uibrushes.h>

namespace UI
{
    namespace Graphics
    {
        namespace Brushes
        {
            SolidColorBrush::SolidColorBrush(const Base::Color & _col):
                col(_col)
            {
            }

            void SolidColorBrush::perpare()
            {
                glColor4f(col.x, col.y, col.z, col.t);
            }

            void SolidColorBrush::sample(double x, double y, const Math::Vec2 & pt)
            {
                glVertex2d(pt.x, pt.y);
            }

            GradientBrush::GradientBrush(const Base::Color & _col0, const Base::Color & _col1, const Base::Color & _col2, const Base::Color & _col3):
                col0(_col0), col1(_col1), col2(_col2), col3(_col3)
            {
            }

            void GradientBrush::parpare()
            {
            }

            void GradientBrush::sample(double x, double y, const Math::Vec2 & pt)
            {
                //Linear interpolation 
                Base::Color colleft = (col3 - col0) * ((y + 1.0) / 2) + col0;
                Base::Color colright = (col2 - col1) * ((y + 1.0) / 2) + col1;
                Base::Color col = (colright - colleft) * ((x + 1.0) / 2) + colleft;
                glColor4f(col.x, col.y, col.z, col.t);
                glVertex2d(pt.x, pt.y);
            }
        }
    }
}
