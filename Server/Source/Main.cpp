#include <thread>
#include <mutex>
#include <algorithm>
#include <vector>
#include <map>
#include "Network.h"
#include "Console.h"
#include "..\..\source\PlayerPacket.h"
using std::map;
using std::vector;
using std::thread;

map<int, PlayerPacket> players;
std::mutex m;

void handle(std::shared_ptr<ip::tcp::socket> sock)
{
    unsigned int onlineID = 0;
    bool IDSet = false;
    m.lock();
    Print("New connection. Online players:" + toString(players.size()+1));
    m.unlock();
    while (true)
    {
        int signal[1];
        try
        {
            sock->read_some(buffer(signal));
        }
        catch (std::exception&)
        {
            Print("A connection closed.");
            m.lock();
            players.erase(onlineID);
            m.unlock();
            return;
        }
        int len[1];
        sock->read_some(buffer(len));
        vector<char> data(len[0]);
        m.lock();
        Print("Online players:" + toString(players.size()));
        switch (signal[0])
        {
        case PLAYER_PACKET_SEND:
        {
            //客户端玩家数据更新
            PlayerPacket* pp = (PlayerPacket*)data.data();
            if (IDSet&&pp->onlineID != onlineID)
            {
                Print("The packet is trying to change other player's data. May cheat? (Packet from " + toString(onlineID) + ")", MESSAGE_WARNING);
                break;
            }
            map<int, PlayerPacket>::iterator iter = players.find(pp->onlineID);
            if (iter == players.end())
            {
                if (IDSet)
                {
                    Print("Can't find player data, may change the online id in game. (" + toString(onlineID) + " to " + toString(pp->onlineID) + ")", MESSAGE_WARNING);
                    break;
                }
                players[pp->onlineID] = *pp;  //第一次上传数据
                IDSet = true;
                onlineID = pp->onlineID;
            }
            else
            {
                if (!IDSet)
                {
                    Print("May repeat login?", MESSAGE_WARNING);
                    break;
                }
                iter->second = *pp;  //更新数据
            }
            break;
        }
        case PLAYER_PACKET_REQ:
        {
            //客户端请求其他玩家的位置
            if (players.size() == 0) break;
            char buf[sizeof(PlayerPacket)];
            for (map<int, PlayerPacket>::iterator it = players.begin(); it != players.end(); it++)
            {
                memcpy(buf, &it->second, sizeof(PlayerPacket));
                sock->write_some(buffer(buf));
            }
            break;
        }
        }
        m.unlock();
    }
}

int main()
{
    Print("NEWorld Server 0.2.1(Dev.) for NEWorld Alpha 0.5.0(Dev.). Using the developing version to play is not recommended.");
    Print("The server is starting...");
    Network::Init();
    Print("The server is running.");
    vector<std::thread> clients;
    while (true)
    {
        clients.push_back(std::thread(handle, Network::GetNextClient()));
    }
    Print("The server is stopping...");
    std::for_each(clients.begin(), clients.end(), [](std::thread& t)
    {
        t.join();
    });
    Network::Clean();
    system("pause");
    return 0;
}
