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

#include <uibase.h>
#include <uilogger.h>
#include <GL/glew.h>
#include <SDL2/SDL_image.h>

namespace UI
{
    namespace Core
    {
        extern SDL_Renderer* currenderer;
    }

    namespace Base
    {
        Color blend(const Color & lhs, const Color & rhs)
        {
            return Color(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z, lhs.t*rhs.t);
        }

        void init()
        {
            int initted = IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
            if (!initted)
            {
                logwarning("IMG_Init: Failed to init required jpg and png support!\n");
                logwarning("IMG_Init:" + std::string(IMG_GetError()));
            }
        }

        Rect::Rect(double _xmin, double _xmax, double _ymin, double _ymax) :xmin(_xmin), xmax(_xmax), ymin(_ymin), ymax(_ymax)
        {
        }

        Rect::Rect(Math::Vec2 p1, Math::Vec2 p2) :
            xmin(p1.x), ymin(p1.y), xmax(p2.x), ymax(p2.y)
        {
        }

        void Rect::set(double _xmin, double _xmax, double _ymin, double _ymax)
        {
            xmin = _xmin;
            xmax = _xmax;
            ymin = _ymin;
            ymax = _ymax;
        }

        Texture::Texture(std::string path)
        {
            SDL_Surface *image = IMG_Load(path.c_str());
            if (!image) logwarning("IMG_Load:" + std::string(IMG_GetError()));
            size_t wid = (size_t)pow(2, ceil(log2(image->h > image->w ? image->h : image->w)));
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            GLubyte* Tex = new GLubyte[wid * wid * 4];
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, wid, 0, GL_RGBA, GL_UNSIGNED_BYTE, Tex);
            glTexSubImage2D(texture, 0, 0, 0, image->w, image->h, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
        }

        Texture::~Texture()
        {
        }

        Image::Image()
        {
        }

        Image::Image(Image & img)
        {
        }

        Image::Image(std::string path)
        {
        }

        Image Image::Sub(Rect range)
        {
            return Image();
        }

        Image::~Image()
        {
        }

        void Brush::perpare()
        {
        }
    }
}
