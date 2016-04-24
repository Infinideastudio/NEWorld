#ifndef NETWORK_H
#define NETWORK_H

#include <memory>
#define ASIO_STANDALONE
#include <asio/asio.hpp>
using namespace asio;

namespace Network
{
    const unsigned short port = 30001;
    void Init();
    void Clean();
    std::shared_ptr<ip::tcp::socket> GetNextClient();

    //Private Section
    extern io_service m_ios;
    extern ip::tcp::acceptor* m_server;
}

#endif
