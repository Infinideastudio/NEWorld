#ifndef NETWORK_H
#define NETWORK_H

#include "Definitions.h"
#define ASIO_STANDALONE
#include <asio/asio.hpp>
using namespace asio;

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
