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

#include <iostream>
#include <string>
#include <boost/dll/shared_library.hpp>
#include <common.h> // For NWAPICALL

const std::string Path = "./Bin/";
typedef void NWAPICALL MainFunction(int, char**);

int main(int argc, char** argv)
{
    std::cout << "Welcome to NEWorld Minimal Launcher!" << std::endl;
    std::cout << "Enter 'client' to run client" << std::endl;
    std::cout << "Enter 'server' to run server" << std::endl;
    std::string in;
    std::cin >> in;
    try
    {
        if (in == "server")
            boost::dll::shared_library
            (
                Path + "NEWorldServer",
                boost::dll::load_mode::append_decorations
            )
            .get<MainFunction>("main")(argc, argv);
        else
            boost::dll::shared_library
            (
                Path + "NEWorld",
                boost::dll::load_mode::append_decorations
            )
            .get<MainFunction>("main")(argc, argv);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        system("pause");
    }
    return 0;
}
