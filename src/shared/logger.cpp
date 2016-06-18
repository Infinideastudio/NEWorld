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

#include "precomp.h"
#include <iomanip>
#include <ctime>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "logger.h"

std::vector<std::ofstream> Logger::fsink;
int Logger::clogLevel = trace;
int Logger::cerrLevel = fatal;
int Logger::fileLevel = trace;

string Logger::getTimeString(char dateSplit, char midSplit, char timeSplit)
{
    time_t timer = time(NULL);
    tm* currtime = localtime(&timer); // DO NOT `delete` THIS POINTER!
    std::stringstream ss;
    ss << std::setfill('0')
       << std::setw(4) << currtime->tm_year + 1900 << dateSplit << std::setw(2) << currtime->tm_mon + 1 << dateSplit << std::setw(2) << currtime->tm_mday
       << midSplit << std::setw(2) << currtime->tm_hour << timeSplit << std::setw(2) << currtime->tm_min << timeSplit << std::setw(2) << currtime->tm_sec;
    return ss.str();
}

void loggerInit()
{
    using namespace boost::filesystem;
    string path = "./Logs/";
    if (!exists(path)) create_directory(path);
    Logger::addFileSink(path);
    Logger::clogLevel = Logger::debug;
    Logger::cerrLevel = Logger::fatal;
    Logger::fileLevel = Logger::debug;
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
