/*
 * NEWorld: An free game with similar rules to Minecraft.
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

#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include "Definitions.h"

class TextRenderer
{
public:
    struct UnicodeChar
    {
        bool aval;
        TextureID tex;
        VBOID buffer;
        unsigned int vtxs;
        int xpos, ypos;
        int width, height, advance;
        UnicodeChar() :aval(false), tex(0), buffer(0),
            vtxs(0), xpos(0), ypos(0),
            width(0), height(0), advance(0) {}
    };

    static FT_Library library;
    static FT_Face fontface;
    static FT_GlyphSlot slot;
    static UnicodeChar chars[65536];
    static map<string, wchar_t*> wstr_cache;
    const static int max_cache_size = 1024;

    static unsigned int gbe, Font;
    static int gloop, ww, wh;
    static float r, g, b, a;

    static void BuildFont(int w, int h);
    static void resize()
    {
        assert(!FT_Set_Pixel_Sizes(fontface, static_cast<int>(16 * stretch), static_cast<int>(16 * stretch)));
        for (int i = 0; i < 63356; i++)
            if (chars[i].aval)
            {
                chars[i].aval = false;
                glDeleteTextures(1, &chars[i].tex);
            }
    }
    static void setFontColor(float r_, float g_, float b_, float a_)
    {
        r = r_;
        g = g_;
        b = b_;
        a = a_;
    }
    static void loadchar(unsigned int uc);
    static int getStrWidth(string s);
    static void renderString(int x, int y, string glstring);
    static void renderASCIIString(int x, int y, string glstring);
    static void clearCache();
};

#endif
