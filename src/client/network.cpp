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
#include <networkstructures.h>

std::string hostIp = "127.0.0.1";
boost::asio::io_service ioService;
std::shared_ptr<Session> session;
const int updateInterval = 10;
const int Port = 8090; //TODO: read it from a address
void disconnect()
{
    ioService.stop();
}

bool initNetwork(boost::asio::io_service& ioservice, tcp::socket& socket, std::string ip)
{
    tcp::resolver resolver(ioservice);
    try
    {
        //Connect to the server
        boost::asio::connect(socket, resolver.resolve({ip, std::to_string(Port)}));
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
    tcp::socket socket(ioService);
    if (!initNetwork(ioService, socket, hostIp))
        exit(-1);

    session = std::make_shared<Session>(std::move(socket));
    session->start();
    session->addRequest(std::move(LoginPacket("test", "123456", NEWorldVersion).makePacket()));
    ioService.run();
}

void errorHandle(const tcp::socket&, boost::system::error_code ec)
{
    errorstream << "Network error, code:" << ec.value();
}

void Session::doUpdate()
{
    auto self(shared_from_this());
    boost::asio::deadline_timer(m_socket.get_io_service(), boost::posix_time::microseconds(updateInterval)).async_wait(
        [this, self](boost::system::error_code)
        {
            //Update world here
            doWrite();
        });
}
