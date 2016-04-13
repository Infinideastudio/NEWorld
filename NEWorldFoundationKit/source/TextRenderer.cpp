#include "TextRenderer.h"
#include "Textures.h"

namespace TextRenderer
{
FT_Library library;
FT_Face fontface;
FT_GlyphSlot slot;
UnicodeChar chars[65536];
unsigned int gbe;
unsigned int Font;
int gloop;
int ww, wh;
float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;
bool useUnicodeASCIIFont;

void BuildFont(int w, int h)
{
    ww = w;
    wh = h;
    Font = Textures::LoadFontTexture("Fonts/ASCII.bmp");

    float cx, cy;
    gbe = glGenLists(256);
    glBindTexture(GL_TEXTURE_2D, Font);
    for (gloop = 0; gloop < 256; gloop++)
    {

        cx = (float)(gloop % 16) / 16.0f;
        cy = (float)(gloop / 16) / 16.0f;

        glNewList(gbe + gloop, GL_COMPILE);
        glBegin(GL_QUADS);
        glTexCoord2f(cx, 1.0f - cy);
        glVertex2i(0, 0);
        glTexCoord2f(cx + 0.0625f, 1.0f - cy);
        glVertex2i(16, 0);
        glTexCoord2f(cx + 0.0625f, 1.0f - cy - 0.0625f);
        glVertex2i(16, 16);
        glTexCoord2f(cx, 1.0f - cy - 0.0625f);
        glVertex2i(0, 16);
        glEnd();
        glTranslated(10.0, 0.0, 0.0);
        glEndList();

    }

    if (FT_Init_FreeType(&library))
    {
        //assert(false);
    }
    if (FT_New_Face(library, "Fonts/Font.ttf", 0, &fontface))
    {
        //assert(false);
    }
    if (FT_Set_Pixel_Sizes(fontface, 16 * stretch, 16 * stretch))
    {
        //assert(false);
    }
    slot = fontface->glyph;

}

void resize()
{
    if (FT_Set_Pixel_Sizes(fontface, 16 * stretch, 16 * stretch))
    {
        //assert(false);
    }
    for (int i = 0; i < 63356; i++) if (chars[i].aval)
        {
            chars[i].aval = false;
            glDeleteTextures(1, &chars[i].tex);
        }
}

void setFontColor(float r_, float g_, float b_, float a_)
{
    r = r_;
    g = g_;
    b = b_;
    a = a_;
}

void loadchar(unsigned int uc)
{
    FT_Bitmap* bitmap;
    unsigned int index;
    ubyte *Tex, *Texsrc;
    int wid = (int)pow(2, ceil(log2(32 * stretch)));

    index = FT_Get_Char_Index(fontface, uc);
    FT_Load_Glyph(fontface, index, FT_LOAD_DEFAULT);
    FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
    bitmap = &(slot->bitmap);
    Texsrc = bitmap->buffer;
    Tex = new ubyte[wid * wid * 4];
    memset(Tex, 0, wid * wid * 4 * sizeof(ubyte));
    for (unsigned int i = 0; i < bitmap->rows; i++)
    {
        for (unsigned int j = 0; j < bitmap->width; j++)
        {
            Tex[(i * wid + j) * 4 + 0] = Tex[(i * wid + j) * 4 + 1] = Tex[(i * wid + j) * 4 + 2] = 255U;
            Tex[(i * wid + j) * 4 + 3] = *Texsrc;
            Texsrc++;
        }
    }
    glGenTextures(1, &chars[uc].tex);
    glBindTexture(GL_TEXTURE_2D, chars[uc].tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, wid, 0, GL_RGBA, GL_UNSIGNED_BYTE, Tex);
    delete[] Tex;
    chars[uc].aval = true;
    chars[uc].width = bitmap->width;
    chars[uc].height = bitmap->rows;
    chars[uc].advance = slot->advance.x >> 6;
    chars[uc].xpos = slot->bitmap_left;
    chars[uc].ypos = slot->bitmap_top;
}

void MBToWC(const char* lpcszStr, wchar_t*& lpwszStr, int dwSize)
{
    lpwszStr = (wchar_t*)malloc(dwSize);
    memset(lpwszStr, 0, dwSize);
    int iSize = (MByteToWChar(lpwszStr, lpcszStr, strlen(lpcszStr)) + 1)*sizeof(wchar_t);
    lpwszStr = (wchar_t*)realloc(lpwszStr, iSize);
}

int getStrWidth(string s)
{
    UnicodeChar c;
    int uc, res = 0;
    wchar_t* wstr = nullptr;
    MBToWC(s.c_str(), wstr, s.length()+128);
    for (unsigned int k = 0; k < wstrlen(wstr); k++)
    {
        uc = wstr[k];
        c = chars[uc];
        if (!c.aval)
        {
            loadchar(uc);
            c = chars[uc];
        }
        res += c.advance / stretch;
    }
    free(wstr);
    return res;
}

void renderString(int x, int y, string glstring)
{
    UnicodeChar c;
    int uc;
    int span = 0;
    double wid = pow(2, ceil(log2(32 * stretch)));
    wchar_t* wstr = nullptr;
    MBToWC(glstring.c_str(), wstr, glstring.length()+128);

    glEnable(GL_TEXTURE_2D);
    for (unsigned int k = 0; k < wstrlen(wstr); k++)
    {

        uc = wstr[k];
        c = chars[uc];
        if (uc == (int)'\n')
        {
            UITrans(0, 20);
            span = 0;
            continue;
        }
        if (!c.aval)
        {
            loadchar(uc);
            c = chars[uc];
        }

        glBindTexture(GL_TEXTURE_2D, c.tex);

        UITrans(x + 1 + span, y + 1);
        glColor4f(0.5, 0.5, 0.5, a);
        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0);
        UIVertex(c.xpos / stretch, 15 - c.ypos / stretch);
        glTexCoord2d(c.width / wid, 0.0);
        UIVertex(c.xpos / stretch + c.width / stretch, 15- c.ypos / stretch);
        glTexCoord2d(c.width / wid, c.height / wid);
        UIVertex(c.xpos / stretch + c.width / stretch, 15 + c.height / stretch - c.ypos / stretch);
        glTexCoord2d(0.0, c.height / wid);
        UIVertex(c.xpos / stretch, 15 + c.height / stretch - c.ypos / stretch);
        glEnd();

        UITrans(-1, -1);
        glColor4f(r, g, b, a);
        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0);
        UIVertex(c.xpos / stretch, 15 - c.ypos / stretch);
        glTexCoord2d(c.width / wid, 0.0);
        UIVertex(c.xpos / stretch + c.width / stretch, 15 - c.ypos / stretch);
        glTexCoord2d(c.width / wid, c.height / wid);
        UIVertex(c.xpos / stretch + c.width / stretch, 15 + c.height / stretch - c.ypos / stretch);
        glTexCoord2d(0.0, c.height / wid);
        UIVertex(c.xpos / stretch, 15 + c.height / stretch - c.ypos / stretch);
        glEnd();

        UITrans(-x - span, -y);
        span += c.advance / stretch;
    }
    glColor4f(1.0, 1.0, 1.0, 1.0);
    free(wstr);
}

void renderASCIIString(int x, int y, string glstring)
{
    //glBindTexture(GL_TEXTURE_2D, Font);
    glPushMatrix();
    glLoadIdentity();
    glColor4f(0.5, 0.5, 0.5, a);
    glTranslated(x + 1, y + 1, 0);
    glListBase(gbe);
    glCallLists((GLsizei)glstring.length(), GL_UNSIGNED_BYTE, glstring.c_str());
    glLoadIdentity();
    glColor4f(r, g, b, a);
    glTranslated(x, y, 0);
    glListBase(gbe);
    glCallLists((GLsizei)glstring.length(), GL_UNSIGNED_BYTE, glstring.c_str());
    glPopMatrix();
}

}
