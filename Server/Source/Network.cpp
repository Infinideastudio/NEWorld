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
