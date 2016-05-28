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

#include "controls.h"

void Label::render()
{
}

Label::Label(std::string _xName, Margin _Margin, std::string _caption):
    Control(_xName, _Margin), caption(_caption)
{
}

void Button::render()
{
}

void Button::mouseButtonFunc(MouseButton Button, ButtonAction Action)
{
}

Button::Button(std::string _xName, Margin _Margin, std::string _caption, NotifyFunc _onClick):
    Control(_xName, _Margin), caption(_caption), onClick(_onClick)
{
}

void TextBox::render()
{
}

void TextBox::mouseButtonFunc(MouseButton Button, ButtonAction Action)
{
}

void TextBox::charInputFunc(wchar_t Char)
{
}

TextBox::TextBox(std::string _xName, Margin _Margin, std::wstring _text, std::string _mask, NotifyFunc _onEditDone) :
    Control(_xName, _Margin), text(_text), mask(_mask), onEditDone(_onEditDone)
{
}

void CheckBox::render()
{
}

void CheckBox::mouseButtonFunc(MouseButton Button, ButtonAction Action)
{
}

CheckBox::CheckBox(std::string _xName, Margin _Margin, bool _threeStat, CheckStat _stat, NotifyFunc _onStatChange) :
    Control(_xName, _Margin), threeStatus(_threeStat), stat(_stat), onStatChange(_onStatChange)
{
}

void Grid::addControls(std::vector<GridChildRecord> newControls)
{
}

void Grid::deleteControl(std::string name)
{
}

GridChildRecord * Grid::getControl(std::string name)
{
    return nullptr;
}

void Grid::render()
{
}

void Grid::focusFunc(FocusOp Stat)
{
}

void Grid::mouseButtonFunc(MouseButton Button, ButtonAction Action)
{
}

void Grid::cursorPosFunc(double x, double y)
{
}

void Grid::crusorEnterFunc(CursorOp Stat)
{
}

void Grid::scrollFunc(double dx, double dy)
{
}

void Grid::keyFunc(int Key, ButtonAction Action)
{
}

void Grid::charInputFunc(wchar_t Char)
{
}

void Grid::dropFunc(int DropCount, const char ** Paths)
{
}

Grid::Grid(std::string _xName, Margin _Margin, std::vector<GridChildRecord> newControls)
{
}

Page::Page(std::string _xName)
{
}
