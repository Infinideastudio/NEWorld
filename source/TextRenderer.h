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
};

#endif
