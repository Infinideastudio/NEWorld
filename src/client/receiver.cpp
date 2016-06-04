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
#include <network.h>
#include <logger.h>
#include <memory>
std::string hostIp;

void setServerIp(std::string ip)
{
    hostIp = ip;
}

std::unique_ptr<NetworkStructure> readData(tcp::socket& s, Identifier identifier)
{
    switch (identifier)
    {
    case Chat: //example
        uint32_t length1, length2;
        std::string username, content;
        boost::asio::read(s, boost::asio::buffer(&length1, sizeof(uint32_t)));
        boost::asio::read(s, boost::asio::buffer(&length2, sizeof(uint32_t)));
        boost::asio::read(s, boost::asio::buffer(username, length1));
        boost::asio::read(s, boost::asio::buffer(content, length2));
        return std::make_unique<ChatPacket>(username, content);
    }
    return nullptr;
}

void receiverThread()
{
    tcp::socket s(ioService);
    tcp::resolver resolver(ioService);
    try
    {
        //connect to the server
        boost::asio::connect(s, resolver.resolve({ hostIp, std::to_string(Port) }));
        while (true)
        {
            //Read the identifier
            Identifier identifier;
            boost::asio::read(s, boost::asio::buffer(&identifier, sizeof(Identifier)));
            readData(s, identifier)->process();
        }
    }
    catch (std::exception& e)
    {
        errorstream << "Exception: " << e.what();
    }
}
