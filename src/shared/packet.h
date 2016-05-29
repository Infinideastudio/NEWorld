#ifndef __PACKET_H_
#define __PACKET_H_
#include "networkstructures.h"
struct Packet
{
    Identifier identifier;
    uint32_t length;
    void* data;
};

#endif
