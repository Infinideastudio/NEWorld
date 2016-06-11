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

#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <utility>
#include <string>
#include "server.h"
#include <common.h>
#include <logger.h>

std::string copyright()
{
    return CopyrightString;
}

int main(int argc, char* argv[])
{
    loggerInit();
    infostream << copyright();
    infostream << "Server is starting... Version: " << NEWORLD_VERSION;
    infostream << "Server started";
    try
    {
        Server s(ioService, Port);
        ioService.run();
    }
    catch (std::exception& e)
    {
        fatalstream << "Exception: " << e.what();
    }
    infostream << "Server is stoping...";
    return 0;
}
