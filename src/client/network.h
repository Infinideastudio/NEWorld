#ifndef NETWORK_H__
#define NETWORK_H__
#include <networkshared.h>
#include <string>
#include <mutex>
class Packet;
void addRequest(Packet p);
void networkThread();
void disconnect();

inline void setServerIp(std::string ip)
{
    extern std::string hostIp;
    hostIp = ip;
}

#endif // NETWORK_H__
