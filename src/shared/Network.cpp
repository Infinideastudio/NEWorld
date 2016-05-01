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
#endif
