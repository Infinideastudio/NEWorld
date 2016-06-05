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

#ifndef SERVER_H__
#define SERVER_H__
#include <network.h>
#include <logger.h>
#include <memory>
#include <session.h>
constexpr int updateInterval = 10, globalUpdateInterval = 10;

class Server
{
public:
    Server(boost::asio::io_service& ioservice, short port)
        : m_acceptor(ioservice, boost::asio::ip::tcp::endpoint(tcp::v4(), port)),
          m_socket(ioservice)
    {
        doAccept();
    }

private:
    void doAccept();
    void doGlobalUpdate();

    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
};

#endif // SERVER_H__
