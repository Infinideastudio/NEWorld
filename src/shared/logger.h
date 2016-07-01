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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
using std::string;
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/bitwise.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/string.hpp>
#include "common.h"

template <bool r, bool g, bool b, bool i, bool f>
class ConsoleColor {};

template <bool r, bool g, bool b, bool i, bool f>
inline std::ostream& operator<< (std::ostream& orig, ConsoleColor<r, g, b, i, f>)
{
#ifdef NEWORLD_TARGET_WINDOWS
#ifdef NEWORLD_USE_WINAPI
    using namespace boost::mpl;
    using namespace CColor;
    if (f)
    {
        // Foreground
        using col = bitor_<
                    integral_c<WORD, r ? FOREGROUND_RED : 0u>,
                    integral_c<WORD, g ? FOREGROUND_GREEN : 0u>,
                    integral_c<WORD, b ? FOREGROUND_BLUE : 0u>,
                    integral_c<WORD, i ? FOREGROUND_INTENSITY : 0u>
                    >;
        fg = col::value;
    }
    else
    {
        // Background
        using col= bitor_<
                   integral_c<WORD, r ? BACKGROUND_RED : 0u>,
                   integral_c<WORD, g ? BACKGROUND_GREEN : 0u>,
                   integral_c<WORD, b ? BACKGROUND_BLUE : 0u>,
                   integral_c<WORD, i ? BACKGROUND_INTENSITY : 0u>
                   >;
        bg = col::value;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bg | fg);
    return orig;
#endif
#else
    // *nix
    // NOT FINISHED
    return orig << "\033[;m";
#endif
}

namespace CColor
{
    extern unsigned short fg, bg;
    // *** Foreground colors ***
    // Grayscales
    typedef ConsoleColor<false, false, false, false, true> black;
    typedef ConsoleColor<false, false, false, true, true> dgray;
    typedef ConsoleColor<true, true, true, false, true> gray;
    typedef ConsoleColor<true, true, true, true, true> white;
    // Bright colors
    typedef ConsoleColor<true, false, false, true, true> red;
    typedef ConsoleColor<false, true, false, true, true> green;
    typedef ConsoleColor<false, false, true, true, true> blue;
    typedef ConsoleColor<true, true, false, true, true> yellow;
    typedef ConsoleColor<false, true, true, true, true> cyan;
    typedef ConsoleColor<true, false, true, true, true> magenta;
    // Dark colors
    typedef ConsoleColor<true, false, false, false, true> dred;
    typedef ConsoleColor<false, true, false, false, true> dgreen;
    typedef ConsoleColor<false, false, true, false, true> dblue;
    typedef ConsoleColor<true, true, false, false, true> dyellow;
    typedef ConsoleColor<false, true, true, false, true> dcyan;
    typedef ConsoleColor<true, false, true, false, true> dmagenta;

    // *** Background colors ***
    // Grayscales
    typedef ConsoleColor<false, false, false, false, false> bblack;
    typedef ConsoleColor<false, false, false, true, false> bdgray;
    typedef ConsoleColor<true, true, true, false, false> bgray;
    typedef ConsoleColor<true, true, true, true, false> bwhite;
    // Bright colors
    typedef ConsoleColor<true, false, false, true, false> bred;
    typedef ConsoleColor<false, true, false, true, false> bgreen;
    typedef ConsoleColor<false, false, true, true, false> bblue;
    typedef ConsoleColor<true, true, false, true, false> byellow;
    typedef ConsoleColor<false, true, true, true, false> bcyan;
    typedef ConsoleColor<true, false, true, true, false> bmagenta;
    // Dark colors
    typedef ConsoleColor<true, false, false, false, false> bdred;
    typedef ConsoleColor<false, true, false, false, false> bdgreen;
    typedef ConsoleColor<false, false, true, false, false> bdblue;
    typedef ConsoleColor<true, true, false, false, false> bdyellow;
    typedef ConsoleColor<false, true, true, false, false> bdcyan;
    typedef ConsoleColor<true, false, true, false, false> bdmagenta;
}

class LoggerStream
{
public:
    explicit LoggerStream(bool cerr) :m_cerr(cerr) {}

    template <typename T>
    LoggerStream& operator<< (const T& rhs)
    {
        if (m_cerr)
            std::cerr << rhs;
        else
            std::clog << rhs;
        m_content << rhs;
        return *this;
    }

    // Get content string
    string get()
    {
        return m_content.str();
    }

private:
    bool m_cerr;
    std::stringstream m_content;
};

// Critical levels
enum Level
{
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};

extern int clogLevel; // Minimum critical level using std::clog and output to console
extern int cerrLevel; // Minumum critical level using std::cerr and output to console
extern int fileLevel; // Minumum critical level output to file
extern int lineLevel; // Minumum critical level output the line number of the source file

//specifying if the logger only outputs infomation to files
extern bool fileOnly;

extern std::vector<std::ofstream> fsink;

string getTimeString(char dateSplit, char midSplit, char timeSplit);

// Add a file sink named with current system time
inline void addFileSink(const string& path, const string& prefix);

template <int level>
class Logger
{
public:
    Logger(const char* fileName, int lineNumber) :m_content(level >= cerrLevel)
    {
        using namespace boost;
        using namespace boost::mpl;
        // Level names
        using LevelString = mpl::vector<mpl::string<'t','r','a','c','e'>, mpl::string<'d','e','b','u','g'>, mpl::string<'i','n','f','o'>, mpl::string<'w','a','r','n','i','n','g'>, mpl::string<'e','r','r','o','r'>, mpl::string<'f','a','t','a','l'>> ;
        // Level colors
        using LevelColor = mpl::vector<CColor::dgray, CColor::gray, CColor::white, CColor::yellow, CColor::red, CColor::dred> ;

        using Color = typename mpl::at_c<LevelColor, level>::type;
        using LevelStr = typename at_c <LevelString, level >::type;
        using StrRightPart = typename push_back < LevelStr, char_<']'> >::type; // "info]"
        using StrFinal = typename push_front < StrRightPart, char_<'['> >::type; // "[info]"

        m_content << CColor::dgray() << CColor::bblack() << '[' << getTimeString('-', ' ', ':') << ']' << Color() << c_str<StrFinal>::value;
        if (level >= lineLevel) m_content << CColor::dgray() << "(" << fileName << ":" << lineNumber << ") ";
        m_content << CColor::gray();
    }

    ~Logger()
    {
        if (!fileOnly)
        {
            if (level >= cerrLevel)
                std::cerr << std::endl;
            else if (level >= clogLevel)
                std::clog << std::endl;
        }
        if (level >= fileLevel)
            for (auto &it : fsink)
                it << m_content.get() << std::endl;
    }

    template <typename T>
    Logger& operator<< (const T& rhs)
    {
        m_content << rhs;
        return *this;
    }


private:
    LoggerStream m_content;

};

void loggerInit(const std::string& prefix);

// information for developers
#define debugstream Logger<debug>(__FUNCTION__, __LINE__)
// information for common users
#define infostream Logger<info>(__FUNCTION__, __LINE__)
// problems that may affect facility, performance or stability but don't lead the game to crash immediately
#define warningstream Logger<warning>(__FUNCTION__, __LINE__)
// the game crashes, but can be resumed by ways such as reloading the world which don't restart the program
#define errorstream Logger<error>(__FUNCTION__, __LINE__)
// unrecoverable error and program termination is required
#define fatalstream Logger<fatal>(__FUNCTION__, __LINE__)

#endif // !LOGGER_H_
