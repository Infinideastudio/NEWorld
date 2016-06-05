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

#include <memory>
#include <common.h>
#include <logger.h>
#include "network.h"
#include "session.h"

std::string hostIp = "127.0.0.1";

std::shared_ptr<Session> session;

void disconnect()
{
    //TODO: disconnect
}

bool initNetwork(boost::asio::io_service& ioService, tcp::socket& socket, std::string ip)
{
    tcp::resolver resolver(ioService);
    try
    {
        //Connect to the server
        boost::asio::connect(socket, resolver.resolve({ ip, std::to_string(Port) }));
        return true;
    }
    catch (std::exception& e)
    {
        fatalstream << "Exception: " << e.what();
        return false;
    }
}

void networkThread()
{
    boost::asio::io_service ioService;
    tcp::socket socket(ioService);
    if (!initNetwork(ioService,socket, hostIp))
        exit(-1);

    session = std::make_shared<Session>(std::move(socket));
    session->start();
    session->addRequest(LoginPacket::make("test", "123456", NEWORLD_VERSION));
    ioService.run();
}

void errorHandle(const tcp::socket& m_socket, boost::system::error_code ec)
{
    errorstream << "Network error, code:" << ec.value();
}

void Session::doUpdate()
{
}
