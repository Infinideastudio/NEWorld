/*
* This file is part of NGWorld.
* Then DLaboratory copied this file from NGWorld to NEWorld.
* (C) Copyright 2016 DLaboratory
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

#include "server.h"

using namespace boost::asio;
using namespace boost::system;

void errorHandle(const tcp::socket& m_socket, error_code ec)
{
    infostream << m_socket.remote_endpoint().address().to_string() << " disconnected, code:" << ec.value();
}

void Session::doUpdate()
{
    auto self(shared_from_this());
    deadline_timer(m_socket.get_io_service(), boost::posix_time::microseconds(updateInterval)).async_wait(
        [this, self](error_code)
    {
        //Update world here
        doWrite();
    });
}

void Server::doAccept()
{
    m_acceptor.async_accept(m_socket, [this](boost::system::error_code ec)
    {
        if (!ec)
        {
            infostream << m_socket.remote_endpoint().address().to_string() << " connects to the server";
            std::make_shared<Session>(std::move(m_socket))->start();
        }
        doAccept();
    });
}

void Server::doGlobalUpdate()
{
    boost::asio::deadline_timer(m_socket.get_io_service(), boost::posix_time::microseconds(globalUpdateInterval)).async_wait(
        [this](boost::system::error_code)
    {
        //Update world here
        doGlobalUpdate();
    });
}
