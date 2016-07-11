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

#ifndef UIBASE_H_
#define UIBASE_H_

#include "uimath.h"
#include <string>
#include <memory>
#include <SDL2/SDL.h>

namespace UI
{
    namespace Base
    {
        using Color = Math::Vec4;
        UILIB_API Color blend(const Color& lhs, const Color& rhs);
        class UILIB_API Rect
        {
        public:
            double xmin, xmax, ymin, ymax;
            Rect() = default;
            Rect(Math::Vec2 p1, Math::Vec2 p2);
            Rect(double _xmin, double _xmax, double _ymin, double _ymax);
            void set(double _xmin, double _xmax, double _ymin, double _ymax);
        };

        class UILIB_API Texture
        {
        public:
            Texture() = default;
            Texture(std::string path);
            ~Texture();
        private:
            unsigned int texture;
        };

        class UILIB_API Image
        {
        public:
            Image();
            Image(Image& img);
            Image(std::string path);
            Image Sub(Rect range);
            ~Image();
        private:
            std::shared_ptr<Texture> tex;
        };

        class UILIB_API Brush
        {
        public:
            virtual void perpare();
            virtual void sample(double x, double y, const Math::Vec2& pt) = 0;
            virtual ~Brush() = default;
        };
    }
}
#endif
