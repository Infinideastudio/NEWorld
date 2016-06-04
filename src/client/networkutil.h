#ifndef NETWORKUTIL_H__
#define NETWORKUTIL_H__
#include <string>
#include <mutex>
extern boost::asio::io_service ioService;
extern tcp::socket globalSocket;
extern std::mutex networkMutex; //mutex

bool initNetwork();

inline void setServerIp(std::string ip)
{
    extern std::string hostIp;
    hostIp = ip;
}

#endif // NETWORKUTIL_H__
