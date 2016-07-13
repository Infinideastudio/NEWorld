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

#ifndef UICONTROLS_H_
#define UICONTROLS_H_

#include "uicore.h"
#include "uilocale.h"

namespace UI
{
    namespace Controls
    {
        using namespace Base;
        using namespace Core;
        using namespace Font;
        using namespace Globalization;
        ////////////////////////////////////////////////////////////
        //    Basic Control Definitions
        ////////////////////////////////////////////////////////////
        class UILIB_API Label : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<FontRenderer> Font;
            Str caption;
            void render();
            Label() = default;
            Label(std::string _xName, Margin _Margin, Str _caption);
        };

        class UILIB_API Button : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<Brush> backgroundHighlightBrush;
            std::shared_ptr<Brush> borderHighlightBrush;
            std::shared_ptr<Brush> backgroundOnPressBrush;
            std::shared_ptr<Brush> borderOnPressBrush;
            std::shared_ptr<FontRenderer> Font;

            NotifyFunc onClick;

            Str caption;
            void render();
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);

            Button() = default;
            Button(std::string _xName, Margin _Margin, Str _caption, NotifyFunc _onClick);
        };

        class UILIB_API TextBox : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<Brush> backgroundHighlightBrush;
            std::shared_ptr<Brush> borderHighlightBrush;
            std::shared_ptr<Brush> crusorBrush;
            std::shared_ptr<FontRenderer> Font;

            NotifyFunc onEditDone;
            std::wstring text;
            Str mask;

            void render();
            void focusFunc(FocusOp Stat);
            void charInputFunc(std::wstring Char);
            void keyFunc(int Key, ButtonAction Action);
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);

            TextBox() = default;
            TextBox(std::string _xName, Margin _Margin, std::wstring _text, Str _mask, NotifyFunc _onEditDone);
        private:
            int left;
            int cursorpxpos = 0;
            size_t cursorpos = 0;
            size_t cposrdcount = 0;
            std::wstring appendingstr;
        };

        enum CheckStat
        {
            Unchecked = 0, Checked, UnKnown
        };

        class UILIB_API CheckBox : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<Brush> backgroundHighlightBrush;
            std::shared_ptr<Brush> borderHighlightBrush;
            std::shared_ptr<Brush> checkBrush;
            std::shared_ptr<Brush> checkHeightLightBrush;
            std::shared_ptr<FontRenderer> Font;

            NotifyFunc onStatChange;

            bool threeStatus;
            CheckStat stat;
            Str caption;

            void render();
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);

            CheckBox() = default;
            CheckBox(std::string _xName, Margin _Margin, Str _caption, bool _threeStat, CheckStat _stat, NotifyFunc _onStatChange);
        };

        class UILIB_API GLContext : public Control
        {
        public:
            GLContext() = default;
            GLContext(std::string _xName, Margin _Margin);

            NotifyFunc onRenderF;
            onFocus onFocusF;
            onViewResize onViewResizeF;
            onMouseMove onMouseMoveF;
            onMouseButton onMouseButtonF;
            onMouseEnter onMouseEnterF;
            onScroll onScrollF;
            onKeyPress onKeyPressF;
            onKeyDown onKeyDownF;
            onFileDrop onFileDropF;
            onTouch onTouchF;
            onTouchMove onTouchMoveF;

            void render();
            void onResize(size_t x, size_t y);
            void onParentResize(Grid* parent);
            void focusFunc(FocusOp Stat);
            void mouseMove(int x, int y, int dx, int dy);
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);
            void crusorEnterFunc(CursorOp Stat);
            void scrollFunc(double dx, double dy);
            void keyFunc(int Key, ButtonAction Action);
            void keyDownFunc(int scancode);
            void dropFunc(const char* Paths);
            void touchFunc(int x, int y, ButtonAction action);
            void touchmove(int x, int y, int dx, int dy);
        };

        class UILIB_API ImageBox : public Control
        {
        public:
            ImageBox() = default;
            ImageBox(std::string _xName, Margin _Margin, const Image& image);
            Image img;
            void render();
        };
    }
}

#endif
