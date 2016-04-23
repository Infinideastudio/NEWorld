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

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Definitions.h"

#ifdef NEWORLD_TARGET_WINDOWS
namespace Network
{
class Request
{
public:
    Request(const char* dataSend, int dataLen, int signal, bool important = false) :
        _dataSend(dataSend), _dataLen(dataLen), _signal(signal), _important(important) {}
    Request(const char* dataSend, int dataLen, int signal, std::function<void(void*, int)> callback, bool important = false) :
        _dataSend(dataSend), _dataLen(dataLen), _signal(signal), _callback(callback), _important(important) {}
    friend ThreadFunc networkThread(void*);
    bool isImportant()
    {
        return _important;
    }
private:
    int _signal;
    const char* _dataSend;
    int _dataLen;
    bool _important;
    std::function<void(void*, int)> _callback;
};
extern mutex_t mutex;
void init(string ip, unsigned short port);
int getRequestCount();
SOCKET getClientSocket();
ThreadFunc networkThread(void*);
void pushRequest(Request& r);
void cleanUp();
}
#endif

#endif
