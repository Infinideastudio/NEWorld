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

#include "TextRenderer.h"
#include "Textures.h"

FT_Library TextRenderer::library;
FT_Face TextRenderer::fontface;
FT_GlyphSlot TextRenderer::slot;
TextRenderer::UnicodeChar TextRenderer::chars[65536];
map<string, wchar_t*> TextRenderer::wstr_cache;
unsigned int TextRenderer::gbe, TextRenderer::Font;
int TextRenderer::gloop, TextRenderer::ww, TextRenderer::wh;
float TextRenderer::r = 0.0f, TextRenderer::g = 0.0f, TextRenderer::b = 0.0f, TextRenderer::a = 1.0f;

void TextRenderer::BuildFont(int w, int h)
{
    ww = w;
    wh = h;
    Font = Textures::LoadFontTexture("Fonts/ASCII.bmp");

    gbe = glGenLists(256);
    glBindTexture(GL_TEXTURE_2D, Font);
    for (gloop = 0; gloop < 256; gloop++)
    {
        float cx = (float)(gloop % 16) / 16.0f, cy = (float)(gloop / 16) / 16.0f;

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

    FT_Init_FreeType(&library);
    FT_New_Face(library, "Fonts/Font.ttf", 0, &fontface);
    FT_Set_Pixel_Sizes(fontface, static_cast<FT_UInt>(16 * stretch), static_cast<FT_UInt>(16 * stretch));
    slot = fontface->glyph;
}

void TextRenderer::loadchar(unsigned int uc)
{
    size_t index = FT_Get_Char_Index(fontface, uc);
    FT_Load_Glyph(fontface, index, FT_LOAD_DEFAULT);
    FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
    FT_Bitmap* bitmap = &(slot->bitmap);
    int wid = (int)pow(2, ceil(log2(32 * stretch)));
    ubyte *Texsrc = bitmap->buffer, *Tex = new ubyte[wid * wid * 4];
    memset(Tex, 0, wid * wid * 4 * sizeof(ubyte));
    for (size_t i = 0; i < bitmap->rows; i++)
    {
        for (size_t j = 0; j < bitmap->width; j++)
        {
            size_t tmp = (i * wid + j) << 2;
            Tex[tmp] = Tex[tmp ^ 1] = Tex[tmp ^ 2] = 255U;
            Tex[tmp ^ 3] = *Texsrc;
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
    dwSize *= sizeof(wchar_t);
    lpwszStr = (wchar_t*)malloc(dwSize);
    memset(lpwszStr, 0, dwSize);
    int iSize = (MByteToWChar(lpwszStr, lpcszStr, strlen(lpcszStr) + 5) + 1)*sizeof(wchar_t);
    if(iSize > dwSize)
        lpwszStr = (wchar_t*)realloc(lpwszStr, iSize);
}

int TextRenderer::getStrWidth(string s)
{
    UnicodeChar c;
    int res = 0;
    wchar_t* wstr = nullptr;
    if(wstr_cache.size() > max_cache_size)
        clearCache();
    if(wstr_cache.find(s) == wstr_cache.end())
    {
        MBToWC(s.c_str(), wstr, s.length()+128);
        wstr_cache[s] = wstr;
    }
    else
    {
        wstr = wstr_cache[s];
    }
    for (unsigned int k = 0; k < wstrlen(wstr); k++)
    {
        int uc = wstr[k];
        c = chars[uc];
        if (!c.aval)
        {
            loadchar(uc);
            c = chars[uc];
        }
        res += static_cast<int>(c.advance / stretch);
    }
    return res;
}

void TextRenderer::renderString(int x, int y, string glstring)
{
    UnicodeChar c;
    int span = 0;
    double wid = pow(2, ceil(log2(32 * stretch)));
    wchar_t* wstr = nullptr;
    if(wstr_cache.size() > max_cache_size)
        clearCache();
    if(wstr_cache.find(glstring) == wstr_cache.end())
    {
        MBToWC(glstring.c_str(), wstr, glstring.length()+128);
        wstr_cache[glstring] = wstr;
    }
    else
    {
        wstr = wstr_cache[glstring];
    }
    glEnable(GL_TEXTURE_2D);
    for (unsigned int k = 0; k < wstrlen(wstr); k++)
    {
        int uc = wstr[k];
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
        span += static_cast<int>(c.advance / stretch);
    }
    glColor4f(1.0, 1.0, 1.0, 1.0);
}

void TextRenderer::renderASCIIString(int x, int y, string glstring)
{
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

void TextRenderer::clearCache()
{
    for(map<string, wchar_t*>::iterator it = wstr_cache.begin(); it != wstr_cache.end(); ++it)
        free(it->second);
    wstr_cache.clear();
}
