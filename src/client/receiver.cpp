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
#include <mutex>
#include "networkutil.h"

std::unique_ptr<NetworkStructure> readData(tcp::socket& s, Identifier identifier)
{
    switch (identifier)
    {
    case Chat: //example
        uint32_t length1, length2;
        boost::asio::read(s, boost::asio::buffer(&length1, sizeof(uint32_t)));
        boost::asio::read(s, boost::asio::buffer(&length2, sizeof(uint32_t)));
        std::unique_ptr<char[]> unBuffer(new char[length1]);
        std::unique_ptr<char[]> cnBuffer(new char[length2]);
        boost::asio::read(s, boost::asio::buffer(unBuffer.get(), length1));
        boost::asio::read(s, boost::asio::buffer(cnBuffer.get(), length2));
        std::string username = unBuffer.get(), content = cnBuffer.get();
        return std::make_unique<ChatPacket>(username, content);
    }
    return nullptr;
}

void receiverThread()
{
    try
    {
        while (true)
        {
            std::unique_ptr<NetworkStructure> packet;
            {
                std::lock_guard<std::mutex> lg(networkMutex);
                //Read the identifier
                Identifier identifier;
                boost::asio::read(globalSocket, boost::asio::buffer(&identifier, sizeof(Identifier)));
                packet = readData(globalSocket, identifier);
            }
            packet->process();
        }
    }
    catch (std::exception& e)
    {
        fatalstream << "Exception: " << e.what();
    }
}
