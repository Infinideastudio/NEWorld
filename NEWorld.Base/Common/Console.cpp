// 
// Core: Console.cpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#include "Console.h"

#if (__CYGWIN__ || _WIN32)

#include <windows.h>
#include <winbase.h>
#include <consoleapi2.h>

namespace LColorFunc {
    // Microsoft Windows
    static HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    std::ostream& black(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, 0);
        return s;
    }

    std::ostream& lblack(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_INTENSITY);
        return s;
    }

    std::ostream& red(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED);
        return s;
    }

    std::ostream& lred(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
        return s;
    }

    std::ostream& green(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN);
        return s;
    }

    std::ostream& lgreen(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        return s;
    }

    std::ostream& blue(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE);
        return s;
    }

    std::ostream& lblue(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        return s;
    }

    std::ostream& yellow(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN);
        return s;
    }

    std::ostream& lyellow(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        return s;
    }

    std::ostream& magenta(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_BLUE);
        return s;
    }

    std::ostream& lmagenta(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        return s;
    }

    std::ostream& cyan(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_BLUE);
        return s;
    }

    std::ostream& lcyan(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        return s;
    }

    std::ostream& white(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return s;
    }

    std::ostream& lwhite(std::ostream& s) noexcept {
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        return s;
    }
}

#else
    // *nix
namespace LColorFunc {
    std::ostream& black(std::ostream& s) noexcept { return s << "\033[21;30m"; }

    std::ostream& lblack(std::ostream& s) noexcept { return s << "\033[1;30m"; }

    std::ostream& red(std::ostream& s) noexcept { return s << "\033[21;31m"; }

    std::ostream& lred(std::ostream& s) noexcept { return s << "\033[1;31m"; }

    std::ostream& green(std::ostream& s) noexcept { return s << "\033[21;32m"; }

    std::ostream& lgreen(std::ostream& s) noexcept { return s << "\033[1;32m"; }

    std::ostream& blue(std::ostream& s) noexcept { return s << "\033[21;34m"; }

    std::ostream& lblue(std::ostream& s) noexcept { return s << "\033[1;34m"; }

    std::ostream& yellow(std::ostream& s) noexcept { return s << "\033[21;33m"; }

    std::ostream& lyellow(std::ostream& s) noexcept { return s << "\033[1;33m"; }

    std::ostream& magenta(std::ostream& s) noexcept { return s << "\033[21;35m"; }

    std::ostream& lmagenta(std::ostream& s) noexcept { return s << "\033[1;35m"; }

    std::ostream& cyan(std::ostream& s) noexcept { return s << "\033[21;36m"; }

    std::ostream& lcyan(std::ostream& s) noexcept { return s << "\033[1;36m"; }

    std::ostream& white(std::ostream& s) noexcept { return s << "\033[21;37m"; }

    std::ostream& lwhite(std::ostream& s) noexcept { return s << "\033[1;37m"; }

}
#endif