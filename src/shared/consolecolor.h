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

#ifndef CONSOLECOLOR_H_
#define CONSOLECOLOR_H_

#include <iostream>
#include "common.h"

namespace CColor
{

    typedef std::ostream&(*colorfunc)(std::ostream &s);

    enum Color
    {
        red,
        dred,
        yellow,
        white,
        gray
    };

#ifdef NEWORLD_TARGET_WINDOWS
    // Microsoft Windows

    template<Color c>
    inline std::ostream& color(std::ostream& s)
    {
        static_assert(false);
    }

    inline std::ostream& color_(std::ostream& s, WORD attrib)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attrib);
        return s;
    }

    template<>
    inline std::ostream& color<Color::red>(std::ostream& s)
    {
        return color_(s, FOREGROUND_RED|FOREGROUND_INTENSITY);
    }

    template<>
    inline std::ostream& color<Color::dred>(std::ostream& s)
    {
        return color_(s, FOREGROUND_RED);
    }

    template<>
    inline std::ostream& color<Color::yellow>(std::ostream& s)
    {
        return color_(s, FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
    }

    template<>
    inline std::ostream& color<Color::white>(std::ostream& s)
    {
        return color_(s, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }

    template<>
    inline std::ostream& color<Color::gray>(std::ostream& s)
    {
        return color_(s, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
    }
#else
    // *nix

    template<Color c>
    inline std::ostream& color(std::ostream& s)
    {
        static_assert(false);
    }

    template<>
    inline std::ostream& color<Color::red>(std::ostream& s)
    {
        return s << "\033[1;31m";
    }

    template<>
    inline std::ostream& color<Color::dred>(std::ostream& s)
    {
        return s << "\033[21;31m";
    }

    template<>
    inline std::ostream& color<Color::yellow>(std::ostream& s)
    {
        return s << "\033[1;33m";
    }

    template<>
    inline std::ostream& color<Color::white>(std::ostream& s)
    {
        ; return s << "\033[1;37m";
    }

    template<>
    inline std::ostream& color<Color::gray>(std::ostream& s)
    {
        return s << "\033[21;37m";
    }
#endif

} // namespace CColor

#endif // !CONSOLECOLOR_H_
