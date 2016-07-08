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

#include "logger.h"
#include "consolecolor.h"
#include <fstream>
#include <ctime>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <map>

std::vector<std::ofstream> fsink;

int clogLevel = trace;
int cerrLevel = fatal;
int fileLevel = trace;
int lineLevel = warning;

bool fileOnly = false;

static std::string LevelTags[6];

template<size_t length>
static string convert(int arg)
{
    char arr[13];
    int siz = 0u;
    while (arg)
    {
        arr[siz++] = (arg % 10) + '0';
        arg /= 10;
    }
    string ret(length - siz, '0');
    ret.reserve(length);
    for (int i = siz - 1; i >= 0; i--)
        ret += arr[i];
    return ret;
}

string getTimeString(char dateSplit, char midSplit, char timeSplit)
{
    time_t timer = time(nullptr);
    tm* currtime = localtime(&timer); // DO NOT `delete` THIS POINTER!
    return convert<4u>(currtime->tm_year + 1900) + dateSplit + convert<2u>(currtime->tm_mon) + dateSplit + convert<2u>(currtime->tm_mday)
           + midSplit + convert<2u>(currtime->tm_hour) + timeSplit + convert<2u>(currtime->tm_min) +timeSplit + convert<2u>(currtime->tm_sec);
}

inline void addFileSink(const string& path, const string& prefix)
{
    fsink.emplace_back(path + prefix + "_" + getTimeString('-', '_', '-') + ".log");
}

void loggerInit(const string& prefix)
{
    using namespace boost::filesystem;
    const char *path = "./Logs/";
    if (!exists(path))
    {
        create_directories(path);
    }

    addFileSink(path, prefix);

    std::stringstream ss;

    int c = 0;

    ss.str("");
    ss << LColor::white << '[' << "trace" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << LColor::white << '[' << "debug" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << LColor::lwhite << '[' << "info" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << LColor::lyellow << '[' << "warning" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << LColor::lred << '[' << "error" << ']';
    LevelTags[c++] = ss.str();

    ss.str("");
    ss << LColor::red << '[' << "fatal" << ']';
    LevelTags[c] = ss.str();

    // File sequence number not finished
    /*
    directory_iterator itemEnd;
    for (directory_iterator item(path); item != itemEnd; item++)
    if (is_directory(*item))
    {
    string filePath = item->path().string() + "/" + item->path().filename().string();
    }
    */
}

Logger::Logger(const char* fileName, int lineNumber, Level level): m_level(level)
{
    m_content << LColor::white << '[' << getTimeString('-', ' ', ':') << ']' << LevelTags[level];
    if (level >= lineLevel) m_content << "(" << fileName << ":" << lineNumber << ") ";
}

void Logger::writeOstream(std::ostream &ostream,bool noColor) const
{
    using namespace LColorFunc;
    constexpr static char stylechar = '&';
    static std::map<char,colorfunc> cmap =
    {
        {'0',black},{'1',red},{'2',green},{'3',blue},
        {'4',yellow},{'5',magenta},{'6',cyan},{'7',white},
        {'h',lblack},{'a',lred},{'b',lgreen},{'c',lblue},
        {'d',lyellow},{'e',lmagenta},{'f',lcyan},{'g',lwhite},
    };
    std::string str = m_content.str();
    string::size_type pos1 = 0, pos2 = str.find(stylechar);
    while (true)
    {
        if(string::npos == pos2)
        {
            ostream << str.substr(pos1,str.size());
            return;
        }
        ostream << str.substr(pos1, pos2 - pos1);
        if(pos2 < str.size())
        {
            char ch = str[pos2+1];
            if(!noColor)
            {
                colorfunc cf = cmap[ch];
                if(cf) ostream << cf;
                else ostream << '&' << ch;
            }
        }

        pos1 = pos2 + 2;
        pos2 = str.find(stylechar, pos1);
    }
}

Logger::~Logger()
{
    m_content << std::endl;
    if (!fileOnly)
    {
        if (m_level >= cerrLevel)
            writeOstream(std::cerr);
        else if (m_level >= clogLevel)
            writeOstream(std::cout);
    }
    if (m_level >= fileLevel)
        for (auto &it : fsink)
            writeOstream(it,true);
}

NWAPIEXPORT void NWAPICALL consolePrint(const char * str, Level level)
{
    switch (level)
    {
    case trace:
        Logger(__FUNCTION__, __LINE__,trace) << str;
        break;
    case debug:
        debugstream << str;
        break;
    case warning:
        warningstream << str;
        break;
    case error:
        errorstream << str;
        break;
    case fatal:
        fatalstream << str;
        break;
    default:
    case info:
        infostream << str;
        break;
    }
}
