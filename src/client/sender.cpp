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

#include "sender.h"
#include <logger.h>
#include <queue>
#include <mutex>
//The packets which are waiting to send
std::queue<Packet> packets;

//mutex
std::mutex senderMutex;

void senderThread()
{
    extern std::string hostIp;
    tcp::socket s(ioService);
    tcp::resolver resolver(ioService);
    try
    {
        //Connect to the server
        boost::asio::connect(s, resolver.resolve({ hostIp, std::to_string(Port) }));

        //Make a loop to send packets
        while (true)
        {
            senderMutex.lock();

            if (packets.empty()) continue;
            Packet p = packets.front();
            packets.pop();

            senderMutex.unlock();

            boost::asio::write(s, boost::asio::buffer(&p.identifier, sizeof(Identifier)));
            boost::asio::write(s, boost::asio::buffer(&p.length, sizeof(p.length)));
            boost::asio::write(s, boost::asio::buffer(&p.data, p.length));
        }
    }
    catch (std::exception& e)
    {
        errorstream << "Exception: " << e.what();
    }
}

void addRequest(Packet p)
{
    std::lock_guard<std::mutex> lock(senderMutex);
    packets.push(p);
}
