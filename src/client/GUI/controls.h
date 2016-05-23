/*
* NEWorld: A free game with similar rules to Minecraft.
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

#ifndef CONTROLS_H_
#define CONTROLS_H_

#include "Graphics.h"
#include <functional>

class Margin : public UIObject
{
private:
    Rect relativeps, relativepc;

public:
    Margin() {};
    Margin(Rect _relative_ps, Rect _relative_pc) {};
    Rect getAbsolutePos(Rect Parent_Rect) { return Rect(); };
};

enum FocusOp
{
    Lose, Gain
};

enum ButtonAction
{
    Press, Release, Repeat
};

enum MouseButton
{
    Left, Middle, Right, Preserved1, Preserved2
};

enum CursorOp
{
    Leave, Enter
};

class Control :public UIObject
{
private:
    Margin cMargin;
public:
    Rect tempAbsoluteRect;
    Control* parent;
    std::string xName;

    Control() {}
    Control(std::string _xName, Margin _Margin):parent(nullptr) {}

    virtual void focusFunc(FocusOp Stat) = 0;
    virtual void mouseButtonFunc(MouseButton Button, ButtonAction Action) {};
    virtual void cursorPosFunc(double x, double y) {};
    virtual void crusorEnterFunc(CursorOp Stat) {};
    virtual void scrollFunc(double dx, double dy) {};
    virtual void keyFunc(int Key, ButtonAction Action) {};
    virtual void charInputFunc(wchar_t Char) {};
    virtual void dropFunc(int DropCount, const char** Paths) {};

    virtual ~Control() {}
};

using NotifyFunc        = std::function<void(Control* Sender)>;
using OnFocusFunc       = std::function<void(Control* Sender, FocusOp Stat)>;
using OnMouseButtonFunc = std::function<void(Control* Sender, MouseButton Button, ButtonAction Action)>;
using OnCursorPosFunc   = std::function<void(Control* Sender, double x, double y)>;
using OnCrusorEnterFunc = std::function<void(Control* Sender, CursorOp Stat)>;
using OnScrollFunc      = std::function<void(Control* Sender, double dx, double dy)>;
using OnKeyFunc         = std::function<void(Control* Sender, int Key, ButtonAction Action)>;
using OnCharInputFunc   = std::function<void(Control* Sender, wchar_t Char)>;
using OnDropFunc        = std::function<void(Control* Sender, int DropCount, const char** Paths)>;

#endif // !CONTROLS_H_
