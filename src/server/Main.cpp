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
#include <utility>
#include <queue>
#include <boost/core/noncopyable.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
using namespace boost::asio;
using namespace boost::system;
using namespace boost::posix_time;
constexpr int updateInterval = 10, globalUpdateInterval = 10;

void errorHandle(const tcp::socket& m_socket, error_code ec)
{
    infostream << m_socket.remote_endpoint().address().to_string() << " disconnected. Code:" << ec.value();
}

/// A helper class for taking data out of a byte array
class takeDataHelper: boost::noncopyable
{
public:
    takeDataHelper(char* buffer, size_t length, bool autoReleaseArray=false)
        :m_buffer(buffer), m_length(length), m_offset(0), m_autoReleaseArray(autoReleaseArray) {}
    ~takeDataHelper()
    {
        delete[] m_buffer;
    }
    template<typename T>
    T take()
    {
        if (m_offset + sizeof(T) >= m_length) throw;
        T ret = *((T*)(m_buffer + m_offset));
        m_offset += sizeof(T);
        return ret;
    }
    std::string getString(size_t length)
    {
        if (m_offset + length >= m_length) throw;
        char* ret = m_buffer + m_offset;
        m_offset += length;
        return std::string(ret);
    }

private :
    char* m_buffer;
    size_t m_length;
    size_t m_offset;
    bool m_autoReleaseArray;
};

std::unique_ptr<NetworkStructure> makeNetworkStructure(Packet& packet)
{
    takeDataHelper tdh(packet.data, packet.length);
    packet.identifier = tdh.take<Identifier>();
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
            [this, self](error_code)
        {
            //Update world here
            doWrite();
        });
    }

    void doRead()
    {
        auto self(shared_from_this());
        //先异步读取数据包的长度
        async_read(m_socket,buffer(&m_packetRead, sizeof(Identifier)+sizeof(m_packetRead.length)), //read identifier and length to packet
                   [this, self](error_code ec, std::size_t)
        {
            if (!ec)
            {
                //根据读到的长度新建缓存
                m_dataBuffer = new char[m_packetRead.length];
                //异步读取数据
                async_read(m_socket, buffer(&m_dataBuffer, m_packetRead.length),
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
        async_write(m_socket, buffer(&packet.identifier, sizeof(Identifier)+sizeof(packet.length)), //Send identifier and length
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
    Server(boost::asio::io_service& ioservice, short port)
        : m_acceptor(ioservice, tcp::endpoint(tcp::v4(), port)),
          m_socket(ioservice)
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
                infostream << m_socket.remote_endpoint().address().to_string() << " connects to the server";
                std::make_shared<Session>(std::move(m_socket))->start();
            }
            doAccept();
        });
    }

    void doGlobalUpdate()
    {
        deadline_timer(m_socket.get_io_service(), microseconds(globalUpdateInterval)).async_wait(
            [this](error_code)
        {
            //Update world here
            doGlobalUpdate();
        });
    }

    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
};
std::string getCurrentSystemTime()
{
    ptime ct = second_clock::local_time();
    auto date = ct.date();
    auto tim = ct.time_of_day();
    return (boost::format("%d-%02d-%02d %02d:%02d:%02d") % int(date.year()) % int(date.month()) % int(date.day()) % tim.hours() % tim.minutes() % tim.seconds()).str();
}
int main(int argc, char* argv[])
{
    loggerInit();
    infostream << "System time: " << getCurrentSystemTime();
    infostream << "Server is starting...";
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
