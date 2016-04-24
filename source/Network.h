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

#ifndef NETWORK_H
#define NETWORK_H

#include "Definitions.h"
#define ASIO_STANDALONE
#include <asio/asio.hpp>
using namespace asio;

#ifdef NEWORLD_TARGET_WINDOWS
namespace Network
{
    void Init(const string& ip, uint16 port);
    void Clean();

    class Request
    {
    public:
        Request(const vector<char>& _data, int signal) :
            data(_data), _signal(signal) {}
        Request(const vector<char>& _data, int signal, std::function<void(void*, int)> callback) :
            data(_data), _signal(signal), _callback(callback) {}
        int _signal;
        vector<char> data;
        std::function<void(void*, int)> _callback;
    };

    //private section
    extern io_service m_ios;
    extern ip::tcp::socket* m_socket;
    extern thread_t m_thread;
    extern bool m_thread_running;
    extern queue<Request> m_req;

    ThreadFunc NetworkThreadFunc(void*);
}
#endif
