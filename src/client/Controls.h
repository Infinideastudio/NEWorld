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

#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include "Grafics.h"

class Margine : public Object
{
private:
    Rect Relative_ps, Relative_pc;
public:
    Margine(Rect _Relative_ps, Rect _Relative_pc);
    Rect GetAbsolutePos(Rect Parent_Rect);
};

class Control :public Object
{
private:
    Margine CMargine;
public:
    Rect TempAbsoluteRect;
    Control* Parent;
    std::string xName;
    Control();
    Control(std::string xName, Margine _Margine);
    virtual ~Control();
};
#endif // !_CONTROLS_H_