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
#ifdef NEWORLD_TARGET_WINDOWS
namespace Network
{

SOCKET sockClient;
thread_t t;
mutex_t mutex;
std::queue<Request> reqs;
bool threadRun = true;

void init(string ip, unsigned short _port)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(1, 1), &wsaData);
    sockClient = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(_port);
    connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
    threadRun = true;
    mutex = MutexCreate();
    t = ThreadCreate(networkThread, NULL);
}

int getRequestCount()
{
    return reqs.size();
}
SOCKET getClientSocket()
{
    return sockClient;
}

ThreadFunc networkThread(void *)
{
    while (updateThreadRun)
    {
        MutexLock(mutex);
        if (!threadRun)
        {
            MutexUnlock(mutex);
            break;
        }
        if (reqs.empty())
        {
            MutexUnlock(mutex);
            continue;
        }
        Request& r = reqs.front();
        //if (r._signal == PLAYER_PACKET_SEND && ((PlayerPacket*)r._dataSend)->onlineID != player::onlineID)
        //	cout << "[ERROR]WTF!!!" << endl;
        if (r._dataSend != nullptr && r._dataLen != 0)
        {
            char* data = new char[r._dataLen + sizeof(int)];
            int* signal = (int*)data;
            *signal = r._signal;
            memcpy_s(data + sizeof(int), r._dataLen, r._dataSend, r._dataLen);
            send(getClientSocket(), data, r._dataLen + sizeof(int), 0);
            delete[] data;
        }
        else
        {
            send(getClientSocket(), (const char*)&r._signal, sizeof(int), 0);
        }
        if (r._callback)   //判断有无回调函数
        {
            auto callback = r._callback;
            MutexUnlock(mutex);
            char recvBuf[1024]; //接收缓存区
            int len = recv(sockClient, recvBuf, 1024, 0); //获得数据长度
            if (len > 0) callback(recvBuf, len); //调用回调函数
            MutexLock(mutex);
        }
        reqs.pop();
        MutexUnlock(mutex);
    }
    return 0;
}

void pushRequest(Request& r)
{
    if (reqs.size() + 1 > NetworkRequestMax)    //超过请求队列长度，试图精简队列
    {
        if (!reqs.front().isImportant()) reqs.pop();
    }
    if (reqs.size() + 1 > NetworkRequestMax * 2)    //大量超过请求队列长度，只保留重要请求
    {
        std::queue<Request> q;
        while (reqs.size() != 0)
        {
            if (reqs.front().isImportant()) q.push(reqs.front());
            reqs.pop();
        }
        reqs = q;
    }
    reqs.push(r);
}

void cleanUp()
{
    threadRun = false;
    ThreadWait(t);
    ThreadDestroy(t);
    MutexDestroy(mutex);
    closesocket(getClientSocket());
    WSACleanup();
}
}
#endif
