#ifndef NETWORK_H
#define NETWORK_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Definitions.h"

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
