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
class ConsoleColor {};

template <bool r, bool g, bool b, bool i>
inline std::ostream& operator<<(std::ostream& orig, ConsoleColor<r, g, b, i>)
{
#ifdef _WIN32
    using namespace boost::mpl;
    typedef bitor_<integral_c<WORD, r ? FOREGROUND_RED : 0u>,
            integral_c<WORD, g ? FOREGROUND_GREEN : 0u>,
            integral_c<WORD, b ? FOREGROUND_BLUE : 0u>,
            integral_c<WORD, i ? FOREGROUND_INTENSITY : 0u>> col;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col::value);
    return orig;
#else
    // *nix
    // NOT FINISHED
    return orig << "\033[;m";
#endif
}

namespace CColor
{
    // Grayscales
    typedef ConsoleColor<false, false, false, false> black;
    typedef ConsoleColor<false, false, false, true> dgray;
    typedef ConsoleColor<true, true, true, false> gray;
    typedef ConsoleColor<true, true, true, true> white;
    // Bright colors
    typedef ConsoleColor<true, false, false, true> red;
    typedef ConsoleColor<false, true, false, true> green;
    typedef ConsoleColor<false, false, true, true> blue;
    typedef ConsoleColor<true, true, false, true> yellow;
    typedef ConsoleColor<false, true, true, true> cyan;
    typedef ConsoleColor<true, false, true, true> magenta;
    // Dark colors
    typedef ConsoleColor<true, false, false, false> dred;
    typedef ConsoleColor<false, true, false, false> dgreen;
    typedef ConsoleColor<false, false, true, false> dblue;
    typedef ConsoleColor<true, true, false, false> dyellow;
    typedef ConsoleColor<false, true, true, false> dcyan;
    typedef ConsoleColor<true, false, true, false> dmagenta;
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
inline void addFileSink(const string& path, const string& prefix)
{
    fsink.emplace_back(path + prefix + "_" + getTimeString('-', '_', '-') + ".log");
}

template <int level>
class Logger
{
public:
    Logger(const char* fileName, int lineNumber) :m_content(level >= cerrLevel)
    {
        using namespace boost;
        using namespace boost::mpl;
        // Level names
        typedef mpl::vector<mpl::string<'trac', 'e'>, mpl::string<'debu', 'g'>, mpl::string<'info'>, mpl::string<'warn' ,'ing'>, mpl::string<'erro', 'r'>, mpl::string<'fata', 'l'>> LevelString;
        // Level colors
        typedef mpl::vector<CColor::dgray, CColor::gray, CColor::white, CColor::yellow, CColor::red, CColor::dred> LevelColor;

        m_content << CColor::dgray() << '[' << getTimeString('-', ' ', ':') << ']' << typename mpl::at_c<LevelColor, level>::type() <<
            c_str
            <
                typename push_front
                <
                    typename push_back
                    <
                        typename at_c
                        <
                            LevelString,
                            level
                        >::type,
                        char_<']'>
                    >::type,
                    char_<'['>
                >::type
            >::value;
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
