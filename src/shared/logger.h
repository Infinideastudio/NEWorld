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

template <bool r, bool g, bool b, bool i>
class ConsoleForeColor {};
template <bool r, bool g, bool b, bool i>
class ConsoleBackColor {};

// Foreground Color
template <bool r, bool g, bool b, bool i>
inline std::ostream& operator<< (std::ostream& orig, ConsoleForeColor<r, g, b, i>)
{
#ifdef NEWORLD_TARGET_WINDOWS
    // Microsoft Windows
#ifdef NEWORLD_USE_WINAPI
    using namespace boost::mpl;
    using namespace CColor;
    typedef bitor_<integral_c<WORD, r ? FOREGROUND_RED : 0u>,
            integral_c<WORD, g ? FOREGROUND_GREEN : 0u>,
            integral_c<WORD, b ? FOREGROUND_BLUE : 0u>,
            integral_c<WORD, i ? FOREGROUND_INTENSITY : 0u>> col;
    fg = col::value;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bg | fg);
#endif
    return orig;
#else
    // *nix
    // NOT FINISHED
    return orig << "\033[;m";
#endif
}

// Background Color
template <bool r, bool g, bool b, bool i>
inline std::ostream& operator<< (std::ostream& orig, ConsoleBackColor<r, g, b, i>)
{
#ifdef NEWORLD_TARGET_WINDOWS
    // Microsoft Windows
#ifdef NEWORLD_USE_WINAPI
    using namespace boost::mpl;
    using namespace CColor;
    typedef bitor_<integral_c<WORD, r ? BACKGROUND_RED : 0u>,
            integral_c<WORD, g ? BACKGROUND_GREEN : 0u>,
            integral_c<WORD, b ? BACKGROUND_BLUE : 0u>,
            integral_c<WORD, i ? BACKGROUND_INTENSITY : 0u>> col;
    bg = col::value;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bg | fg);
#endif
    return orig;
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
    typedef ConsoleForeColor<false, false, false, false> black;
    typedef ConsoleForeColor<false, false, false, true> dgray;
    typedef ConsoleForeColor<true, true, true, false> gray;
    typedef ConsoleForeColor<true, true, true, true> white;
    // Bright colors
    typedef ConsoleForeColor<true, false, false, true> red;
    typedef ConsoleForeColor<false, true, false, true> green;
    typedef ConsoleForeColor<false, false, true, true> blue;
    typedef ConsoleForeColor<true, true, false, true> yellow;
    typedef ConsoleForeColor<false, true, true, true> cyan;
    typedef ConsoleForeColor<true, false, true, true> magenta;
    // Dark colors
    typedef ConsoleForeColor<true, false, false, false> dred;
    typedef ConsoleForeColor<false, true, false, false> dgreen;
    typedef ConsoleForeColor<false, false, true, false> dblue;
    typedef ConsoleForeColor<true, true, false, false> dyellow;
    typedef ConsoleForeColor<false, true, true, false> dcyan;
    typedef ConsoleForeColor<true, false, true, false> dmagenta;

    // *** Background colors ***
    // Grayscales
    typedef ConsoleBackColor<false, false, false, false> bblack;
    typedef ConsoleBackColor<false, false, false, true> bdgray;
    typedef ConsoleBackColor<true, true, true, false> bgray;
    typedef ConsoleBackColor<true, true, true, true> bwhite;
    // Bright colors
    typedef ConsoleBackColor<true, false, false, true> bred;
    typedef ConsoleBackColor<false, true, false, true> bgreen;
    typedef ConsoleBackColor<false, false, true, true> bblue;
    typedef ConsoleBackColor<true, true, false, true> byellow;
    typedef ConsoleBackColor<false, true, true, true> bcyan;
    typedef ConsoleBackColor<true, false, true, true> bmagenta;
    // Dark colors
    typedef ConsoleBackColor<true, false, false, false> bdred;
    typedef ConsoleBackColor<false, true, false, false> bdgreen;
    typedef ConsoleBackColor<false, false, true, false> bdblue;
    typedef ConsoleBackColor<true, true, false, false> bdyellow;
    typedef ConsoleBackColor<false, true, true, false> bdcyan;
    typedef ConsoleBackColor<true, false, true, false> bdmagenta;
}

class LoggerStream
{
public:
    explicit LoggerStream(bool cerr) :m_cerr(cerr) {}
    ~LoggerStream()
    {
        if (m_cerr)
            std::cerr << m_curr.str() << std::endl;
        else
            std::clog << m_curr.str() << std::endl;
    }

    template <bool r, bool g, bool b, bool i>
    LoggerStream& operator<< (const ConsoleForeColor<r, g, b, i>& rhs)
    {
        if (m_cerr)
            std::cerr << m_curr.str() << rhs;
        else
            std::clog << m_curr.str() << rhs;
        m_content << m_curr.str();
        m_curr.str("");
        return *this;
    }

    template <bool r, bool g, bool b, bool i>
    LoggerStream& operator<< (const ConsoleBackColor<r, g, b, i>& rhs)
    {
        if (m_cerr)
            std::cerr << m_curr.str() << rhs;
        else
            std::clog << m_curr.str() << rhs;
        m_content << m_curr.str();
        m_curr.str("");
        return *this;
    }

    template <typename T>
    LoggerStream& operator<< (const T& rhs)
    {
        m_curr << rhs;
        return *this;
    }

    // Get content string
    string get()
    {
        return m_content.str() + m_curr.str();
    }

private:
    bool m_cerr;
    std::stringstream m_content, m_curr;
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
        using LevelString =
            mpl::vector<
            mpl::string<'t','r','a','c','e'>,
            mpl::string<'d','e','b','u','g'>,
            mpl::string<'i','n','f','o'>,
            mpl::string<'w','a','r','n','i','n','g'>,
            mpl::string<'e','r','r','o','r'>,
            mpl::string<'f','a','t','a','l'>
            >;
        // Level colors
        using LevelColor =
            mpl::vector<
            CColor::dgray,
            CColor::gray,
            CColor::white,
            CColor::yellow,
            CColor::red,
            CColor::dred
            >;
        // Construct prefix string at compilation time
        using Color = typename mpl::at_c<LevelColor, level>::type;
        using Str0 = typename at_c<LevelString, level>::type;      // "info"
        using Str1 = typename push_front<Str0, char_<'['>>::type;  // "[info"
        using Str2 = typename push_back<Str1, char_<']'>>::type;   // "[info]"
        // Output to stringstream
        m_content << CColor::dgray() << CColor::bblack() << '[' << getTimeString('-', ' ', ':') << ']' << Color() << c_str<Str2>::value;
        if (level >= lineLevel) m_content << CColor::dgray() << "(" << fileName << ":" << lineNumber << ")";
        m_content << " " << CColor::gray();
    }

    ~Logger()
    {
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
