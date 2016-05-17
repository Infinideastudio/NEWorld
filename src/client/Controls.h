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

class Margin : public Object
{
private:
    Rect Relative_ps, Relative_pc;
public:
    Margin();
    Margin(Rect _Relative_ps, Rect _Relative_pc);
    Rect GetAbsolutePos(Rect Parent_Rect);
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

class Control :public Object
{
private:
    Margin CMargin;
public:
    Rect TempAbsoluteRect;
    Control* Parent;
    std::string xName;

    Control();
    Control(std::string _xName, Margin _Margin);

    virtual void FocusFunc(FocusOp Stat) = 0;
    virtual void MouseButtonFunc(MouseButton Button, ButtonAction Action) = 0;
    virtual void CursorPosFunc(double x, double y) = 0;
    virtual void CrusorEnterFunc(CursorOp Stat) = 0;
    virtual void ScrollFunc(double dx, double dy) = 0;
    virtual void KeyFunc(int Key, ButtonAction Action) = 0;
    virtual void CharInputFunc(wchar_t Char) = 0;
    virtual void DropFunc(int DropCount, const char** Paths) = 0;

    virtual ~Control();
};

typedef void (Control::*NotifyFunc) (Control* Sender);
typedef void (Control::*OnFocusFunc) (Control* Sender, FocusOp Stat);
typedef void (Control::*OnMouseButtonFunc) (Control* Sender, MouseButton Button, ButtonAction Action);
typedef void (Control::*OnCursorPosFunc) (Control* Sender, double x, double y);
typedef void (Control::*OnCrusorEnterFunc) (Control* Sender, CursorOp Stat);
typedef void (Control::*OnScrollFunc) (Control* Sender, double dx, double dy);
typedef void (Control::*OnKeyFunc) (Control* Sender, int Key, ButtonAction Action);
typedef void (Control::*OnCharInputFunc) (Control* Sender, wchar_t Char);
typedef void (Control::*OnDropFunc) (Control* Sender, int DropCount, const char** Paths);

#endif // !CONTROLS_H_
