// 
// Core: Console.h
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

#pragma once

#include <ostream>

namespace LColorFunc {
    using ColorFunc = std::ostream& (*)(std::ostream& s) noexcept;

    std::ostream& black(std::ostream& s) noexcept;

    std::ostream& lblack(std::ostream& s) noexcept;

    std::ostream& red(std::ostream& s) noexcept;

    std::ostream& lred(std::ostream& s) noexcept;

    std::ostream& green(std::ostream& s) noexcept;

    std::ostream& lgreen(std::ostream& s) noexcept;

    std::ostream& blue(std::ostream& s) noexcept;

    std::ostream& lblue(std::ostream& s) noexcept;

    std::ostream& yellow(std::ostream& s) noexcept;

    std::ostream& lyellow(std::ostream& s) noexcept;

    std::ostream& magenta(std::ostream& s) noexcept;

    std::ostream& lmagenta(std::ostream& s) noexcept;

    std::ostream& cyan(std::ostream& s) noexcept;

    std::ostream& lcyan(std::ostream& s) noexcept;

    std::ostream& white(std::ostream& s) noexcept;

    std::ostream& lwhite(std::ostream& s) noexcept;
}

namespace LColor {
    constexpr const char* black = "&0";
    constexpr const char* red = "&1";
    constexpr const char* yellow = "&2";
    constexpr const char* green = "&3";
    constexpr const char* cyan = "&4";
    constexpr const char* blue = "&5";
    constexpr const char* magenta = "&6";
    constexpr const char* white = "&7";
    constexpr const char* lblack = "&8";
    constexpr const char* lred = "&9";
    constexpr const char* lyellow = "&a";
    constexpr const char* lgreen = "&b";
    constexpr const char* lcyan = "&c";
    constexpr const char* lblue = "&d";
    constexpr const char* lmagenta = "&e";
    constexpr const char* lwhite = "&f";
}