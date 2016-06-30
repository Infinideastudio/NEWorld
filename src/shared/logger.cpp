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

#include <iomanip>
#include <ctime>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "logger.h"

std::vector<std::ofstream> fsink;
WORD CColor::fg = 0u, CColor::bg = 0u;

int clogLevel = trace;
int cerrLevel = fatal;
int fileLevel = trace;
int lineLevel = warning;

bool fileOnly = false;

template<size_t length>
string convert(int arg)
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
    time_t timer = time(NULL);
    tm* currtime = localtime(&timer); // DO NOT `delete` THIS POINTER!
    return convert<4u>(currtime->tm_year + 1900) + dateSplit + convert<2u>(currtime->tm_mon) + dateSplit + convert<2u>(currtime->tm_mday)
           + midSplit + convert<2u>(currtime->tm_hour) + timeSplit + convert<2u>(currtime->tm_min) +timeSplit + convert<2u>(currtime->tm_sec);
}

void loggerInit(const string& prefix)
{
    using namespace boost::filesystem;
    string path = "./Logs/";
    if (!exists(path))
        create_directory(path);
    addFileSink(path, prefix);
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
