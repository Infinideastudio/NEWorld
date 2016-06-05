#ifndef NETWORK_H__
#define NETWORK_H__
#include <string>
#include <mutex>
#include "session.h"

void addRequest(Packet p);
void networkThread();

inline void setServerIp(std::string ip)
{
    extern std::string hostIp;
    hostIp = ip;
}

#endif // NETWORK_H__
