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

#ifndef SERVER_H_
#define SERVER_H_

#include <memory>
#include <networkshared.h>
#include <logger.h>
#include <session.h>
#include <pluginmanager.h>
#include <worldmanager.h>

constexpr int updateInterval = 10, globalUpdateInterval = 10;

class Server
{
public:
    Server(boost::asio::io_service& ioservice, short port)
        :m_acceptor(ioservice, boost::asio::ip::tcp::endpoint(tcp::v4(), port)), m_socket(ioservice),
         m_worlds(m_plugins), m_world(m_worlds.addWorld("TestWorld"))
    {
        // Initialization
        m_plugins.initPluginAPI();
        m_plugins.loadPlugins();
        // Start server
        doAccept();
    }
    ~Server()
    {
        // TODO: Terminate here
    }

private:
    void doAccept();
    void doGlobalUpdate();

    tcp::acceptor m_acceptor;
    tcp::socket m_socket;

    WorldManager m_worlds;
    PluginManager m_plugins; // Loaded plugins
    World& m_world; // Single world, only for debugging

};

#endif // SERVER_H__
