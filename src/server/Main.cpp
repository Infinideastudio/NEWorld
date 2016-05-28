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

class session
    : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket)
        : m_socket(std::move(socket))
    {
    }

    void start()
    {
        doRead();
    }

private:
    void doRead()
    {
        auto self(shared_from_this());
        m_socket.async_read_some(boost::asio::buffer(m_data, PacketMaxLength),
                                 [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                doWrite(length);
            }
            else
            {
                infostream << m_socket.remote_endpoint().address().to_string() << " disconnected" << logendl;
            }
        });
    }

    void doWrite(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(m_socket, boost::asio::buffer(m_data, length),
                                 [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                doRead();
            }
            else
            {
                infostream << m_socket.remote_endpoint().address().to_string() << " disconnected" << logendl;
            }
        });
    }

    tcp::socket m_socket;
};

class server
{
public:
    server(boost::asio::io_service& io_service, short port)
        : m_acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
          m_socket(io_service)
    {
        doAccept();
    }

private:
    void doAccept()
    {
        m_acceptor.async_accept(m_socket, [this](boost::system::error_code ec)
        {
            if (!ec)
            {
                infostream << m_socket.remote_endpoint().address().to_string() << " connects to the server" << logendl;
                std::make_shared<session>(std::move(m_socket))->start();
            }
            doAccept();
        });
    }

    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
};

int main(int argc, char* argv[])
{
    infostream << "Server is starting..." << logendl;
    infostream << "Server started" << logendl;
    try
    {
        server s(io_service, Port);
        io_service.run();
    }
    catch (std::exception& e)
    {
        errorstream << "Exception: " << e.what() << logendl;
    }
    infostream << "Server is stoping..." << logendl;
    return 0;
}
