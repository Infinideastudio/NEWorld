/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <string>
#include <climits>
#include <engine/common.h>

typedef void NWAPICALL MainFunction(int, char**);

#ifdef _WIN32
constexpr const char* GUIDllName = "GUI.dll";
#else
constexpr const char* GUIDllName = "libGUI.so";
#endif

int main(int argc, char** argv) {
    getSettings();
    Logger::addFileSink("./log/", "launcher");
    std::string file = argc == 1 ? GUIDllName : argv[1];
    debugstream << "Load:" << file;
    Library(file).get<MainFunction>("cmain")(argc, argv);
}
