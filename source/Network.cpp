#include "Network.h"

namespace Network
{
    io_service m_ios;
    ip::tcp::socket* m_socket = nullptr;
    thread_t m_thread;
    bool m_thread_running;
    queue<Request> m_req;

    void Init(const string & ip, uint16 port)
    {
        m_socket = new ip::tcp::socket(m_ios);
        m_socket->connect(ip::tcp::endpoint(ip::address::from_string(ip), port));
        m_thread = ThreadCreate(NetworkThreadFunc, nullptr);
        m_thread_running = true;
    }

    void Clean()
    {
        m_thread_running = false;
        ThreadWait(m_thread);
        ThreadDestroy(m_thread);
        delete m_socket;
        m_socket = nullptr;
    }

    ThreadFunc NetworkThreadFunc(void *)
    {
        while (updateThreadRun&&m_thread_running)
        {
            if (!m_req.empty())
            {
                Request cur = m_req.front();
                m_req.pop();
                m_socket->write_some(buffer(vector<int>(1, cur._signal)));
                if (cur.data.size())
                {
                    m_socket->write_some(buffer(vector<int>(1, cur.data.size())));
                    m_socket->write_some(buffer(cur.data));
                }
                if (cur._callback)
                {
                    vector<int> len(1);
                    m_socket->read_some(buffer(len));
                    vector<int> data(len[0]);
                    m_socket->read_some(buffer(data));
                    cur._callback(data.data(), len[0]);
                }
            }
            Sleep(1);
        }
        return 0;
    }
}
