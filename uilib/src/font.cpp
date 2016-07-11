#include <uilib.h>
#include <freetype/ftsizes.h>
#include <boost/filesystem.hpp>
#include <boost/locale/encoding_utf.hpp>
namespace UI
{
    namespace Font
    {
        using ubyte = unsigned char;
        FT_Library library;
        double stretch = 1.0;

        UnicodeChar::UnicodeChar() : tex(0), xpos(0), ypos(0), width(0), height(0), advance(0)
        {
        }

#define MByteToWChar(x) boost::locale::conv::utf_to_utf<wchar_t>(x)
#define WCharToMByte(x) boost::locale::conv::utf_to_utf<char>(x)

        UnicodeChar FontOfSize::GetChar(const wchar_t uc)
        {
            auto it = chars.find(uc);

            if(it != chars.end())
            {
                return it->second;
            }
            else
            {
                UnicodeChar c;
                size_t index = FT_Get_Char_Index(*fontface, static_cast<FT_ULong>(uc));
                FT_Activate_Size(size);
                FT_Load_Glyph(*fontface, index, FT_LOAD_DEFAULT);
                FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
                FT_Bitmap* bitmap = &(slot->bitmap);
                int wid = (int)pow(2, ceil(log2(height)));
                ubyte *Texsrc = bitmap->buffer, *Tex = new ubyte[wid * wid * 4];
                memset(Tex, 0, wid * wid * 4 * sizeof(ubyte));

                for(size_t i = 0; i < bitmap->rows; i++)
                {
                    for(size_t j = 0; j < bitmap->width; j++)
                    {
                        size_t tmp = (i * wid + j) << 2;
                        Tex[tmp] = Tex[tmp ^ 1] = Tex[tmp ^ 2] = 255U;
                        Tex[tmp ^ 3] = *Texsrc;
                        Texsrc++;
                    }
                }

                glGenTextures(1, &c.tex);
                glBindTexture(GL_TEXTURE_2D, c.tex);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, wid, 0, GL_RGBA, GL_UNSIGNED_BYTE, Tex);
                //glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, wid, wid, GL_RGBA, GL_UNSIGNED_BYTE, Tex);
                delete[] Tex;
                c.width = bitmap->width;
                c.height = bitmap->rows;
                c.advance = slot->advance.x >> 6;
                c.xpos = slot->bitmap_left;
                c.ypos = slot->bitmap_top;
                chars.insert({ uc, c });
                return c;
            }
        }

        FontOfSize::FontOfSize(const int _height, FT_Face* _face) :
            height(_height), fontface(_face)
        {
            FT_New_Size(*fontface, &size);
            FT_Activate_Size(size);
            FT_Set_Pixel_Sizes(*fontface, static_cast<FT_UInt>(height), static_cast<FT_UInt>(height));
            slot = (*fontface)->glyph;
        }

        FontRenderer::FontRenderer(FontOfSize & _fos, Color col)
            : fos(_fos), color(col)
        {
        }

        void FontRenderer::renderStr(int x, int y, const Rect& r, const std::wstring & str) const
        {
            UnicodeChar c;
            int span = 0, tpos = fos.height - 1;
            double wid = pow(2, ceil(log2(fos.height)));
            glEnable(GL_TEXTURE_2D);

            for(wchar_t wc : str)
            {
                c = fos.GetChar(wc);
                int p1x = x + span + c.xpos, p1y = y + tpos - c.ypos,
                    p2x = x + span + c.xpos + c.width, p2y = y + tpos + c.height - c.ypos;
                double t1x = 0.0, t1y = 0.0,
                       t2x = c.width / wid, t2y = c.height / wid;
                span += c.advance;

                //Apply Clipping
                if(p1x > r.xmax) continue;   //already exceeds max size

                if(p2x < r.xmin) continue;  //before the rect, clipped

                if(p1x < r.xmin)
                {
                    t1x = t2x * ((r.xmin - p1x) / c.width);
                    p1x = r.xmin;
                }

                if(p2x > r.xmax)
                {
                    t2x *= (1 - (p2x - r.xmax) / c.width);
                    p2x = r.xmax;
                }

                glBindTexture(GL_TEXTURE_2D, c.tex);
                glBegin(GL_QUADS);
                glColor4f(0.5, 0.5, 0.5, color.t);
                glTexCoord2d(t1x, t1y);
                glVertex2i(p1x + 1, p1y + 1);
                glTexCoord2d(t2x, t1y);
                glVertex2i(p2x + 1, p1y + 1);
                glTexCoord2d(t2x, t2y);
                glVertex2i(p2x + 1, p2y + 1);
                glTexCoord2d(t1x, t2y);
                glVertex2i(p1x + 1, p2y + 1);
                glColor4f(color.x, color.y, color.z, color.t);
                glTexCoord2d(t1x, t1y);
                glVertex2i(p1x, p1y);
                glTexCoord2d(t2x, t1y);
                glVertex2i(p2x, p1y);
                glTexCoord2d(t2x, t2y);
                glVertex2i(p2x, p2y);
                glTexCoord2d(t1x, t2y);
                glVertex2i(p1x, p2y);
                glEnd();
            }

            glDisable(GL_TEXTURE_2D);
        }

        void FontRenderer::renderStr(int x, int y, const Rect& r, const std::string & str) const
        {
            renderStr(x, y, r, MByteToWChar(str));
        }

        double FontRenderer::getStrWidth(const std::wstring & str) const
        {
            int res = 0;

            for(wchar_t wc : str)
                res += fos.GetChar(wc).advance;

            return res;
        }

        double FontRenderer::getStrWidth(const std::string & str) const
        {
            return getStrWidth(MByteToWChar(str));
        }

        FontBase::FontBase(std::string _path):
            path(_path)
        {
            FT_New_Face(library, path.c_str(), 0, &fontface);
        }

        std::shared_ptr<FontRenderer> FontBase::getRenderer(int height, Color col)
        {
            if(sizes.find(height) == sizes.end())
                sizes.insert({ height, FontOfSize(height, &fontface) });
            return std::shared_ptr<FontRenderer>(new FontRenderer(sizes[height], col));
        }

        void FontBase::gc()
        {
        }

        Service::Service()
        {
            FT_Init_FreeType(&library);
        }

        void Service::addSearchPaths(std::initializer_list<std::string> _apths)
        {
            for(auto c : _apths)
                searchpaths.push_back(c);
        }

        inline std::shared_ptr<FontRenderer> Service::getRenderer(const std::string & font, int height, Color col)
        {
            if(fonts.find(font) == fonts.end())
            {
                std::string path;

                for(auto c : searchpaths)
                {
                    if(boost::filesystem::exists(c + "/" + font + ".ttf")) path = c + "/" + font + ".ttf";

                    if(boost::filesystem::exists(c + "/" + font + ".TTF")) path = c + "/" + font + ".TTF";

                    if(boost::filesystem::exists(c + "/" + font + ".otf")) path = c + "/" + font + ".otf";

                    if(boost::filesystem::exists(c + "/" + font + ".OTF")) path = c + "/" + font + ".OTF";
                }

                fonts.insert({ font, FontBase(path) });
            }

            return fonts[font].getRenderer(height, col);
        }

        void Service::gc()
        {
        }

        Service service;
    }
}
