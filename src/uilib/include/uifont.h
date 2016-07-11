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

#ifndef UIFONT_H_
#define UIFONT_H_

#include "uibase.h"
#include <map>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace UI
{
    namespace Font
    {
        using namespace Base;
        struct UnicodeChar
        {
            unsigned int tex;
            int xpos, ypos;
            int width, height, advance;
            UnicodeChar();
        };

        class UILIB_API FontOfSize // Font with a specific size
        {
        public:
            UnicodeChar GetChar(const wchar_t);
            FontOfSize() = default;
            FontOfSize(const int _height, FT_Face* _face);
            int height;
        private:
            //int refcount;
            FT_Size size;
            FT_Face* fontface;
            FT_GlyphSlot slot;
            std::map<wchar_t, UnicodeChar> chars;
            std::vector<unsigned int> texBuffers;
            int curBufferID, curBufferPos, maxBufferX, maxBufferY;
        };

        class UILIB_API FontRenderer
        {
        public:
            Color color;
            FontRenderer(FontOfSize& _fos, Color col);
            void renderStr(int x, int y, const Rect& r, const std::wstring& str) const;
            void renderStr(int x, int y, const Rect& r, const std::string& str) const;
            double getStrWidth(const std::wstring& str) const;
            double getStrWidth(const std::string& str) const;
            FontOfSize& fos;
        };

        class UILIB_API FontBase
        {
        public:
            std::string path;
            FontBase() = default;
            FontBase(std::string _path);
            std::shared_ptr<FontRenderer> getRenderer(int height, Color col);
            void gc(); //only call when not enough physical memory
        private:
            std::map<int, FontOfSize> sizes;
            FT_Face fontface;
        };

        class UILIB_API Service
        {
        public:
            Service();
            void addSearchPaths(std::initializer_list<std::string> _apths);
            std::shared_ptr<FontRenderer> getRenderer(const std::string& font, int height, Color col);
            void gc(); //only call when not enough physical memory
        private:
            std::map<std::string, FontBase> fonts;
            std::vector<std::string> searchpaths;
        };

        UILIB_API extern Service service;
    }
}

#endif
