/*
 * NEWorld: An free game with similar rules to Minecraft.
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

#include "Console.h"
#include <time.h>

void Print(string message, int level)
{
    switch (level)
    {
    case MESSAGE_INFO:
        cout << "[INFO]";
        break;
    case MESSAGE_WARNING:
        cout << "[WARNING]";
        break;
    case MESSAGE_ERROR:
        cout << "[ERROR]";
        break;
    }
    auto t = time(NULL);
    static tm lt;
    localtime_s(&lt, &t);
    cout << "["
         << (lt.tm_hour < 10 ? "0" : "") << lt.tm_hour << ":"
         << (lt.tm_min < 10 ? "0" : "") << lt.tm_min << ":"
         << (lt.tm_sec < 10 ? "0" : "") << lt.tm_sec << "] "
         << message << endl;
}

string toString(int i)
{
    char a[12];
    _itoa_s(i, a, 12, 10);
    return string(a);
}
