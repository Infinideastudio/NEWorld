#ifndef __PACKET_H_
#define __PACKET_H_
#include "networkstructures.h"
struct Packet
{
    Identifier identifier;
    int length;
    void* data;
};

#endif
