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
#include <logger.h>
#include <cstdlib>
#include <thread>
#include <utility>
#include <queue>
#include <chrono>
using namespace boost::asio;
using namespace boost::system;
using namespace boost::posix_time;
const int updateInterval = 10;
const int globalUpdateInterval = 10;

void errorHandle(const tcp::socket& m_socket, error_code ec)
{
    infostream << m_socket.remote_endpoint().address().to_string() << " disconnected" << ec.value() << logendl;
}

class takeDataHelper
{
public:
    takeDataHelper(void* buffer, int length) :m_buffer(buffer), m_length(length), m_offset(0) {}
    template<class T>
    T take()
    {
        if (m_offset + sizeof(T) >= m_length) throw;
        T ret = *((T*)((char*)m_buffer + m_offset));
        m_offset += sizeof(T);
        return ret;
    }
    char* getString(size_t length)
    {
        if (m_offset + length >= m_length) throw;
        char* ret = (char*)m_buffer + m_offset;
        m_offset += length;
        return ret;
    }

private :
    void* m_buffer;
    size_t m_length;
    size_t m_offset;
};

std::unique_ptr<NetworkStructure> makeNetworkStructure(Packet& packet)
{
    takeDataHelper tdh(packet.data, packet.length);
    switch (packet.identifier)
    {
    case Chat:
        uint32_t length1 = tdh.take<uint32_t>(), length2 = packet.length - length1;
        std::string username = tdh.getString(length1), content = tdh.getString(length2);
        return std::make_unique<ChatPacket>(username, content);
    }
    return nullptr;
}

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket)
        : m_socket(std::move(socket))
    {
    }

    void start()
    {
        doRead();
        doUpdate();
    }

private:
    void doUpdate()
    {
        auto self(shared_from_this());
        deadline_timer(m_socket.get_io_service(), microseconds(updateInterval)).async_wait(
            [this, self](error_code ec)
        {
            //Update world here
            doWrite();
        });
    }

    void doRead()
    {
        auto self(shared_from_this());
        async_read(m_socket,buffer(&m_packetRead, sizeof(Identifier)+sizeof(m_packetRead.length)), //read identifier and length to packet
                   [this, self](error_code ec, std::size_t)
        {
            if (!ec)
            {
                m_dataBuffer = new char[m_packetRead.length];
                auto self(shared_from_this());
                async_read(m_socket, buffer(&m_dataBuffer, m_packetRead.length), //read data
                           [this, self](error_code ec, std::size_t)
                {
                    if (!ec)
                    {
                        //Process here
                        makeNetworkStructure(m_packetRead)->process();
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

    void doWrite()
    {
        if (m_packets.empty()) //No packet need sent
        {
            doUpdate();
            return;
        }
        Packet& packet = m_packets.front();
        m_packets.pop();
        auto self(shared_from_this());
        async_write(m_socket, buffer(&packet.identifier, sizeof(Identifier)+sizeof(packet.length)), //Send identifier
                    [this, self, &packet](error_code ec, std::size_t)
        {
            if (!ec)
            {
                async_write(m_socket, buffer(packet.data, packet.length),  //Send data
                            [this, self](error_code ec, std::size_t)
                {
                    if (!ec) doUpdate();
                    else errorHandle(m_socket, ec);
                });
            }
            else errorHandle(m_socket, ec);
        });
    }

    void addRequest(Packet packet)
    {
        m_packets.push(packet);
    }

    tcp::socket m_socket;
    std::queue<Packet> m_packets; //packets need sent
    Packet m_packetRead;
    char* m_dataBuffer;
};

class Server
{
public:
    Server(boost::asio::io_service& ioService, short port)
        : m_acceptor(ioService, tcp::endpoint(tcp::v4(), port)),
          m_socket(ioService)
    {
        doAccept();
    }

private:
    void doAccept()
    {
        m_acceptor.async_accept(m_socket, [this](error_code ec)
        {
            if (!ec)
            {
                infostream << m_socket.remote_endpoint().address().to_string() << " connects to the server" << logendl;
                std::make_shared<Session>(std::move(m_socket))->start();
            }
            doAccept();
        });
    }

    void doGlobalUpdate()
    {
        deadline_timer(m_socket.get_io_service(), microseconds(globalUpdateInterval)).async_wait(
            [this](error_code ec)
        {
            //Update world here
            doGlobalUpdate();
        });
    }

    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
};
const std::string getCurrentSystemTime()
{
    auto tt = std::chrono::system_clock::to_time_t
              (std::chrono::system_clock::now());
    struct tm* ptm = localtime(&tt);
    char date[60] = { 0 };
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
            (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
            (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}
int main(int argc, char* argv[])
{
    infostream << "System time: " << getCurrentSystemTime() << logendl;
    infostream << "Server is starting..." << logendl;
    infostream << "Server started" << logendl;
    try
    {
        Server s(ioService, Port);
        ioService.run();
    }
    catch (std::exception& e)
    {
        errorstream << "Exception: " << e.what() << logendl;
    }
    infostream << "Server is stoping..." << logendl;
    return 0;
}
