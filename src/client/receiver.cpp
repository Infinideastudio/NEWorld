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

#include "receiver.h"
#include "network.h"
#include "logger.h"
std::string hostIp;

void setServerIp(std::string ip)
{
    hostIp = ip;
}

void receiverThread()
{
    boost::asio::io_service io_service;

    tcp::socket s(io_service);
    tcp::resolver resolver(io_service);
    try
    {
        //connect to the server
        boost::asio::connect(s, resolver.resolve({ hostIp, std::to_string(Port) }));
        while (true)
        {
            //Read the identifier
            Identifier identifier;
            boost::asio::read(s, boost::asio::buffer(&identifier, sizeof(Identifier)));

        }
    }
    catch (std::exception& e)
    {
        errorstream << "Exception: " << e.what() << logendl;
    }
}


