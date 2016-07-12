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

#include <iostream>
#include <vector>
#include <sstream>
using std::string;
#include "common.h"

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

class Logger
{
public:
    Logger(const char* fileName, int lineNumber, Level level);
    ~Logger();

    template <typename T>
    Logger& operator<<(const T& rhs)
    {
        m_content << rhs;
        return *this;
    }

private:
    std::stringstream m_content;
    Level m_level;

    void writeOstream(std::ostream& ostream, bool noColor = false) const;
};

void loggerInit(const std::string& prefix);

// information for developers
#define debugstream Logger(__FUNCTION__, __LINE__, debug)
// information for common users
#define infostream Logger(__FUNCTION__, __LINE__, info)
// problems that may affect facility, performance or stability but don't lead the game to crash immediately
#define warningstream Logger(__FUNCTION__, __LINE__, warning)
// the game crashes, but can be resumed by ways such as reloading the world which don't restart the program
#define errorstream Logger(__FUNCTION__, __LINE__, error)
// unrecoverable error and program termination is required
#define fatalstream Logger(__FUNCTION__, __LINE__, fatal)

extern "C"
{
    NWAPIEXPORT void NWAPICALL consolePrint(char* str, Level level);
}

#endif // !LOGGER_H_
