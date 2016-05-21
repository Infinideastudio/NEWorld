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

#include <boost/asio.hpp>
#include "..\shared\Shared.h"
#include <cstdlib>
#include <thread>
#include <utility>

using boost::asio::ip::tcp;

const int MaxLength = 1024;

const int Port = 8090;

void session(tcp::socket sock)
{
    try
    {
        while(true)
        {
            char data[MaxLength];
            boost::system::error_code error;
            size_t length = sock.read_some(boost::asio::buffer(data), error);
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            boost::asio::write(sock, boost::asio::buffer(data, length));
        }
    }
    catch (std::exception& e)
    { logError("Exception: " + std::string(e.what())); }

    logInfo(sock.remote_endpoint().address().to_string() + " disconnected");
}

int main(int argc, char* argv[])
{
    getGlobalLogger().setDisplayToConsole(true);
    logInfo("Server starts...");
    logInfo("Server started");
    try
    {
        boost::asio::io_service io_service;
        tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), Port));
        while (true)
        {
            tcp::socket sock(io_service);
            a.accept(sock);
            logInfo(sock.remote_endpoint().address().to_string() + " connects to the server");
            std::thread(session, std::move(sock)).detach();
        }
    }
    catch (std::exception& e)
    { logError("Exception: " + std::string(e.what())); }
    getGlobalLogger().log("Server is stoping...");
    return 0;
}