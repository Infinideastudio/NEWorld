#pragma once
#include "Definitions.h"

namespace TextRenderer
{
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

extern FT_Library library;
extern FT_Face fontface;
extern FT_GlyphSlot slot;
extern UnicodeChar chars[65536];

extern unsigned int gbe;
extern unsigned int Font;
extern int gloop;
extern int ww, wh;
extern float r, g, b, a;
extern unsigned int unicodeTex[256];
extern bool unicodeTexAval[256];
extern bool UseUnicodeASCIIFont;

void BuildFont(int w, int h);
void resize();
void setFontColor(float r, float g, float b, float a);
void loadchar(unsigned int uc);
int getStrWidth(string s);
void renderString(int x, int y, string glstring);
void renderASCIIString(int x, int y, string glstring);
}