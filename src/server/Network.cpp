/*
 * NEWorld: An free game with similar rules to Minecraft.
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

#include "Network.h"

namespace Network
{
    io_service m_ios;
    ip::tcp::acceptor* m_server;

    void Init()
    {
        m_server = new ip::tcp::acceptor(m_ios, ip::tcp::endpoint(ip::tcp::v4(), port));
    }

    void Clean()
    {
        delete m_server;
        m_server = nullptr;
    }

    std::shared_ptr<ip::tcp::socket> GetNextClient()
    {
        std::shared_ptr<ip::tcp::socket> ret(new ip::tcp::socket(m_ios));
        //We don't need the `remote endpoint`.
        m_server->accept(*ret.get(), ip::tcp::endpoint());
        return ret;
    }
}
