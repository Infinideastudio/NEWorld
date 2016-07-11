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

#ifndef UIBRUSHES_H_
#define UIBRUSHED_H_

#include "uibase.h"
#include <GL/glew.h>

namespace UI
{
    namespace Graphics
    {
        namespace Brushes
        {
            class UILIB_API SolidColorBrush : public Base::Brush
            {
            public:
                SolidColorBrush(const Base::Color& _col);
                void perpare();
                void sample(double x, double y, const Math::Vec2& pt);
            private:
                Base::Color col;
            };
#define UIMakeSolidColorBrush(x) std::shared_ptr<Base::Brush>(new Graphics::Brushes::SolidColorBrush(x))

            class UILIB_API GradientBrush : public Base::Brush
            {
            public:
                GradientBrush(const Base::Color& _col0, const Base::Color& _col1, const Base::Color& _col2, const Base::Color& _col3);
                void parpare();
                void sample(double x, double y, const Math::Vec2& pt);
            private:
                Base::Color col0, col1, col2, col3;
            };
        }
    }
}
#endif
