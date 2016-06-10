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

#include "session.h"
#include "takedatahelper.h"

using namespace boost::asio;
using namespace boost::system;
using namespace boost::posix_time;

void errorHandle(const tcp::socket& m_socket, error_code ec);

std::unique_ptr<NetworkStructure> makeNetworkStructure(Packet& packet)
{
    takeDataHelper tdh(packet.data.get(), packet.length, false);
    switch (packet.identifier)
    {
    case Login:
    {
        std::string username = tdh.getString(64);
        std::string password = tdh.getString(64);
        uint16_t version = tdh.take<uint16_t>();
        return std::make_unique<LoginPacket>(username, password, version);
    }
    case Chat:
    {
        uint32_t length1 = tdh.take<uint32_t>();
        uint32_t length2 = packet.length - length1;
        std::string username = tdh.getString(length1);
        std::string content = tdh.getString(length2);
        return std::make_unique<ChatPacket>(username, content);
    }
    }
    return nullptr;
}

void Session::doRead()
{
    auto self(shared_from_this());
    //先异步读取数据包的长度
    async_read(m_socket, buffer(&m_packetRead, sizeof(Identifier) + sizeof(m_packetRead.length)), // Read identifier and length to packet
               [this, self](error_code ec, std::size_t)
    {
        if (!ec)
        {
            //根据读到的长度新建缓存
            m_packetRead.data = std::unique_ptr<char[]>(new char[m_packetRead.length]);
            //异步读取数据
            async_read(m_socket, buffer(m_packetRead.data.get(), m_packetRead.length),
                       [this, self](error_code ec, std::size_t)
            {
                if (!ec)
                {
                    //处理接收到的数据
                    makeNetworkStructure(m_packetRead)->process();
                    //继续读取其他数据包
                    doRead();
                }
                else
                {
                    errorHandle(m_socket, ec);
                }
            });
        }
        else
        {
            errorHandle(m_socket, ec);
        }
    });
}

void Session::doWrite()
{
    if (m_packets.empty()) // No packet need sent
    {
        doUpdate();
        return;
    }
    Packet& packet = m_packets.front();
    auto self(shared_from_this());
    async_write(m_socket, buffer(&packet.identifier, sizeof(Identifier) + sizeof(packet.length)), // Send identifier and length
                [this, self, &packet](error_code ec, std::size_t)
    {
        if (!ec)
        {
            async_write(m_socket, buffer(packet.data.get(), packet.length), // Send data
                        [this, self](error_code ec, std::size_t)
            {
                m_packets.pop();
                if (!ec) doUpdate();
                else errorHandle(m_socket, ec);
            });
        }
        else errorHandle(m_socket, ec);
    });
}
