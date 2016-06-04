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

#include <network.h>
#include <climits>
#include <logger.h>
#include "networkutil.h"

std::string hostIp = "127.0.0.1";
boost::asio::io_service ioService;
tcp::socket globalSocket(ioService);

std::mutex networkMutex; //mutex

bool initNetwork()
{
    tcp::resolver resolver(ioService);
    try
    {
        //Connect to the server
        boost::asio::connect(globalSocket, resolver.resolve({ hostIp, std::to_string(Port) }));
        return true;
    }
    catch (std::exception& e)
    {
        fatalstream << "Exception: " << e.what();
        return false;
    }
}
