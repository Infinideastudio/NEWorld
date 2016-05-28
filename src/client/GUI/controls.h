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
#include <memory>
#include <map>
#include <vector>

class Margin : public UIObject
{
private:
    Rect relativeps, relativepc;

public:
    Margin() {};
    Margin(Rect _relative_ps, Rect _relative_pc) {};
    Rect getAbsolutePos(Rect Parent_Rect)
    {
        return Rect();
    };
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
protected:
    Margin cMargin;
    bool enabled, mouseOn, pressed, focused;
public:
    Rect tempAbsoluteRect;
    Control* parent;
    std::string xName;

    Control() {}
    Control(std::string _xName, Margin _Margin):parent(nullptr) {}

    virtual void render() {};
    virtual void focusFunc(FocusOp Stat) {};
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

////////////////////////////////////////////////////////////

//    Basic Control Definitions

////////////////////////////////////////////////////////////

class Label :public Control
{
public:
    std::shared_ptr<Brush> backgroundBrush;
    std::shared_ptr<Brush> borderBrush;

    std::string caption;
    void render();
    
    Label() {};
    Label(std::string _xName, Margin _Margin, std::string _caption);
};

class Button :public Control
{
public:
    std::shared_ptr<Brush> backgroundBrush;
    std::shared_ptr<Brush> borderBrush;
    std::shared_ptr<Brush> backgroundHighlightBrush;
    std::shared_ptr<Brush> borderHighlightBrush;
    std::shared_ptr<Brush> backgroundOnPressBrush;
    std::shared_ptr<Brush> borderOnPressBrush;
    
    NotifyFunc onClick;

    std::string caption;
    void render();
    void mouseButtonFunc(MouseButton Button, ButtonAction Action);

    Button() {};
    Button(std::string _xName, Margin _Margin, std::string _caption, NotifyFunc _onClick);
};

class TextBox :public Control
{
public:
    std::shared_ptr<Brush> backgroundBrush;
    std::shared_ptr<Brush> borderBrush;
    std::shared_ptr<Brush> backgroundHighlightBrush;
    std::shared_ptr<Brush> borderHighlightBrush;
    std::shared_ptr<Brush> crusorBrush;

    NotifyFunc onEditDone;

    std::wstring text;
    std::string mask;

    void render();
    void mouseButtonFunc(MouseButton Button, ButtonAction Action);
    void charInputFunc(wchar_t Char);

    TextBox() {};
    TextBox(std::string _xName, Margin _Margin, std::wstring _text, std::string _mask, NotifyFunc _onEditDone);
};

enum CheckStat
{
    Unchecked = 0, Checked, UnKnown
};

class CheckBox :public Control
{
public:
    std::shared_ptr<Brush> backgroundBrush;
    std::shared_ptr<Brush> borderBrush;
    std::shared_ptr<Brush> backgroundHighlightBrush;
    std::shared_ptr<Brush> borderHighlightBrush;
    std::shared_ptr<Brush> checkBrush;
    std::shared_ptr<Brush> checkHeightLightBrush;

    NotifyFunc onStatChange;
    
    bool threeStatus;
    CheckStat stat;

    void render();
    void mouseButtonFunc(MouseButton Button, ButtonAction Action);

    CheckBox() {};
    CheckBox(std::string _xName, Margin _Margin, bool _threeStat, CheckStat _stat, NotifyFunc _onStatChange);
};

struct GridChildRecord
{
    std::shared_ptr<Control> control;
    int gridX, gridY;
};

class Grid : public Control
{
protected:
    std::vector<GridChildRecord> children;   // where the table is actualy stored
    std::map<std::string, int> childReference; // only a reference by xName

public:
    void addControls(std::vector<GridChildRecord> newControls);
    void deleteControl(std::string name);

    GridChildRecord* getControl(std::string name);

    void render();
    void focusFunc(FocusOp Stat);
    void mouseButtonFunc(MouseButton Button, ButtonAction Action);
    void cursorPosFunc(double x, double y);
    void crusorEnterFunc(CursorOp Stat);
    void scrollFunc(double dx, double dy);
    void keyFunc(int Key, ButtonAction Action);
    void charInputFunc(wchar_t Char);
    void dropFunc(int DropCount, const char** Paths);

    Grid() {};
    Grid(std::string _xName, Margin _Margin, std::vector<GridChildRecord> newControls);
};

class Window;

class Page : public Control
{
public:
    Window* parent;
    std::shared_ptr<Grid> content;

    void render();
    void focusFunc(FocusOp Stat);
    void mouseButtonFunc(MouseButton Button, ButtonAction Action);
    void cursorPosFunc(double x, double y);
    void crusorEnterFunc(CursorOp Stat);
    void scrollFunc(double dx, double dy);
    void keyFunc(int Key, ButtonAction Action);
    void charInputFunc(wchar_t Char);
    void dropFunc(int DropCount, const char** Paths);
    
    Page() {};
    Page(std::string _xName);

    virtual ~Page() {};
};
#endif // !CONTROLS_H_
