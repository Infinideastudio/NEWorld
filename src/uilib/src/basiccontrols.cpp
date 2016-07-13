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

#include <uicontrols.h>
#include <uishapes.h>
#include <GL/glew.h>

extern size_t curwindowx;
extern size_t curwindowy;

namespace UI
{
    namespace Controls
    {
        using namespace Graphics::Shapes;
        void Label::render()
        {
            Rectangle::draw(cMargin.absrect, backgroundBrush, borderBrush);
            Font->renderStr((cMargin.absrect.xmin + cMargin.absrect.xmax - Font->getStrWidth((std::string)caption)) / 2,
                            (cMargin.absrect.ymin + cMargin.absrect.ymax - Font->fos.height * 1.25 )/ 2, cMargin.absrect, (std::string)caption);
        }

        Label::Label(std::string _xName, Margin _Margin, Str _caption) :
            Control(_xName, _Margin), caption(_caption)
        {
            gridPosX = gridPosY = 0;
            backgroundBrush = Theme::SystemTheme.ControlBrush;
            borderBrush = nullptr;
            Font = Theme::SystemTheme.DefaultFont;
        }

        void Button::render()
        {
            if (pressed)
                Rectangle::draw(cMargin.absrect, backgroundOnPressBrush, borderOnPressBrush);
            else if (mouseOn)
                Rectangle::draw(cMargin.absrect, backgroundHighlightBrush, borderHighlightBrush);
            else
                Rectangle::draw(cMargin.absrect, backgroundBrush, borderBrush);
            Font->renderStr((cMargin.absrect.xmin + cMargin.absrect.xmax - Font->getStrWidth((std::string)caption)) / 2,
                            (cMargin.absrect.ymin + cMargin.absrect.ymax - Font->fos.height * 1.25) / 2, cMargin.absrect, (std::string)caption);
        }

        void Button::mouseButtonFunc(MouseButton Button, ButtonAction Action)
        {
            if (Action == ButtonAction::Release) onClick();
        }

        Button::Button(std::string _xName, Margin _Margin, Str _caption, NotifyFunc _onClick) :
            Control(_xName, _Margin), caption(_caption), onClick(_onClick),
            backgroundOnPressBrush(Theme::SystemTheme.ControlOnPressBrush),
            backgroundHighlightBrush(Theme::SystemTheme.ControlHeightLightBrush),
            backgroundBrush(Theme::SystemTheme.ControlDarkBrush),
            borderOnPressBrush(Theme::SystemTheme.ActiveBorderBrush),
            borderHighlightBrush(Theme::SystemTheme.ActiveBorderBrush),
            borderBrush(Theme::SystemTheme.InactiveBorderBrush),
            Font(Theme::SystemTheme.DefaultFont)
        {
            gridPosX = gridPosY = 0;
        }

        void TextBox::render()
        {
            const int wid = Font->getStrWidth(text);
            int leftp = (wid >= cMargin.absrect.xmax - 20) ? cMargin.absrect.xmax - wid - 10 : cMargin.absrect.xmin + 10;
            if (-leftp - 10 > cursorpxpos) leftp = -cursorpxpos + 10;
            if (focused)
            {
                cposrdcount++;
                Rectangle::draw(cMargin.absrect, backgroundHighlightBrush, borderHighlightBrush);
                if (cposrdcount > 15)
                {
                    Rect r = cMargin.absrect;
                    r.xmin = leftp + cursorpxpos + 2;
                    r.xmax = r.xmin + 2;
                    r.ymin += 5;
                    r.ymax -= 5;
                    Rectangle::draw(r, crusorBrush, nullptr);
                    if (cposrdcount > 50) cposrdcount = 0;
                }
            }
            else
            {
                Rectangle::draw(cMargin.absrect, backgroundBrush, borderBrush);
            }
            Font->renderStr(leftp, cMargin.absrect.ymin + 2, cMargin.absrect, text);
        }

        SDL_Rect inputRect = { 0,0,800,50 };

        void TextBox::mouseButtonFunc(MouseButton Button, ButtonAction Action)
        {
        }

        void TextBox::charInputFunc(std::wstring Char)
        {
            bool l = cursorpos != 0;
            bool r = cursorpos != text.size();
            text = (l ? text.substr(0, cursorpos) : std::wstring()) + Char + (r ? text.substr(cursorpos) : std::wstring());
            cursorpxpos += Font->getStrWidth(Char);
            cursorpos += Char.size();
        }

        void TextBox::keyFunc(int Key, ButtonAction Action)
        {
            switch (Action)
            {
            case UI::Core::Release:
                if (Key == SDLK_BACKSPACE && !text.empty() && cursorpos!=0)
                {
                    cursorpxpos -= Font->getStrWidth(text.substr(cursorpos - 1, 1));
                    bool l = cursorpos != 0;
                    bool r = cursorpos != text.size();
                    text = (l ? text.substr(0, cursorpos - 1) : std::wstring()) + (r ? text.substr(cursorpos) : std::wstring());
                    cursorpos--;
                }
                else if (Key == SDLK_DELETE && !text.empty() && cursorpos < text.size())
                {
                    bool l = cursorpos != 0;
                    bool r = cursorpos != text.size();
                    text = (l ? text.substr(0, cursorpos) : std::wstring()) + (r ? text.substr(cursorpos + 1) : std::wstring());
                }
                else if (Key == SDLK_LEFT && !text.empty() && cursorpos != 0)
                {
                    cursorpxpos -= Font->getStrWidth(text.substr(cursorpos - 1, 1));
                    cursorpos--;
                }
                else if (Key == SDLK_RIGHT && !text.empty() && cursorpos < text.size())
                {
                    cursorpxpos+= Font->getStrWidth(text.substr(cursorpos, 1));
                    cursorpos++;
                }
                break;
            case UI::Core::Press:
                break;
            case UI::Core::Repeat:
                break;
            default:
                break;
            }
        }

        void TextBox::focusFunc(FocusOp Stat)
        {
            switch (Stat)
            {
            case UI::Core::Lose:
                SDL_StopTextInput();
                break;
            case UI::Core::Gain:
                inputRect.x = cMargin.absrect.xmin;
                inputRect.y = cMargin.absrect.ymax;
                inputRect.w = 500;
                inputRect.h = 50;
                SDL_SetTextInputRect(&inputRect);
                SDL_StartTextInput();
                break;
            default:
                break;
            }
        }

        TextBox::TextBox(std::string _xName, Margin _Margin, std::wstring _text, Str _mask, NotifyFunc _onEditDone) :
            Control(_xName, _Margin), text(_text), mask(_mask), onEditDone(_onEditDone), cursorpxpos(0),cursorpos(0), cposrdcount(0)
        {
            gridPosX = gridPosY = 0;
            backgroundHighlightBrush = Theme::SystemTheme.ControlBrush;
            backgroundBrush = Theme::SystemTheme.ControlBrush;
            borderHighlightBrush = Theme::SystemTheme.ActiveBorderBrush;
            borderBrush = Theme::SystemTheme.InactiveBorderBrush;
            crusorBrush = Theme::SystemTheme.ControlDarkDarkBrush;
            Font = Theme::SystemTheme.DefaultFont;
        }

        void CheckBox::render()
        {
        }

        void CheckBox::mouseButtonFunc(MouseButton Button, ButtonAction Action)
        {
        }

        CheckBox::CheckBox(std::string _xName, Margin _Margin, Str _caption, bool _threeStat, CheckStat _stat, NotifyFunc _onStatChange) :
            Control(_xName, _Margin), threeStatus(_threeStat), stat(_stat), onStatChange(_onStatChange), caption(_caption)
        {
        }

        GLContext::GLContext(std::string _xName, Margin _Margin) :
            Control(_xName, _Margin)
        {
        }

        void GLContext::render()
        {
            glViewport(cMargin.absrect.xmin, cMargin.absrect.ymin,
                       cMargin.absrect.xmax - cMargin.absrect.xmin,
                       cMargin.absrect.ymax - cMargin.absrect.ymin);
            glPushMatrix();
            if (onRenderF) onRenderF();
            glPopMatrix();
            glViewport(0, 0, curwindowx, curwindowy);
        }

        void GLContext::onResize(size_t x, size_t y)
        {
            if (onViewResizeF) onViewResizeF(x, y);
        }

        void GLContext::onParentResize(Grid* parent)
        {
        }

        void GLContext::focusFunc(FocusOp Stat)
        {
            if (onFocusF) onFocusF(Stat);
        }

        void GLContext::mouseMove(int x, int y, int dx, int dy)
        {
            if (onMouseMoveF) onMouseMoveF(x, y, dx, dy);
        }

        void GLContext::mouseButtonFunc(MouseButton Button, ButtonAction Action)
        {
            if (onMouseButtonF) onMouseButtonF(Button, Action);
        }

        void GLContext::crusorEnterFunc(CursorOp Stat)
        {
            if (onMouseEnterF) onMouseEnterF(Stat);
        }

        void GLContext::scrollFunc(double dx, double dy)
        {
            if (onScrollF) onScrollF(dx, dy);
        }

        void GLContext::keyFunc(int Key, ButtonAction Action)
        {
            if (onKeyPressF) onKeyPressF(Key, Action);
        }

        void GLContext::keyDownFunc(int scancode)
        {
            if (onKeyDownF) onKeyDownF(scancode);
        }

        void GLContext::dropFunc(const char * Paths)
        {
            if (onFileDropF) onFileDropF(Paths);
        }

        void GLContext::touchFunc(int x, int y, ButtonAction action)
        {
            if (onTouchF) onTouchF(x, y, action);
        }

        void GLContext::touchmove(int x, int y, int dx, int dy)
        {
            if (onTouchMoveF) onTouchMoveF(x, y, dx, dy);
        }

        ImageBox::ImageBox(std::string _xName, Margin _Margin, const Image & image)
        {
        }

        void ImageBox::render()
        {
        }
    }
}
