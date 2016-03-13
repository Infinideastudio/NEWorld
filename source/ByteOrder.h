#pragma once
#include <stdint.h>

namespace NNN
{

namespace ByteOrder
{

//Returns true if native CPU uses big-endian
bool NativeIsBigEndian()
{
    union
    {
        unsigned long int a;
        unsigned char b[4];
    } c;
    c.a = 0x12345678;
    return (c.b[0] == 0x12);
}

//Convert byte order
template <typename Type>
void convLENative(Type* data, uintptr_t count)
{
    int bytecount = sizeof(Type), j;
    Type sum;
    uint8_t *cur = (uint8_t*)data;
    for (uint32_t i = 0; i < count; i++)
    {
        cur += bytecount;
        sum = 0;
        for (j = 0; j < bytecount; j++)
            sum ^= (Type)cur[j] << (j << 3);
        *(Type*)cur = sum;
    }
}

template <typename Type>
void convNativeLE(Type* data, uintptr_t count)
{
    int bytecount = sizeof(Type), j;
    uint8_t *cur = (uint8_t*)data;
    Type sum;
    for (uint32_t i = 0; i < count; i++)
    {
        cur += bytecount;
        sum = *(Type*)cur;
        for (j = 0; j < bytecount; j++)
            cur[j] = sum & ((Type)0xff << (j << 3));
    }
}

void swapEndiannessU16(uint16_t* data, uintptr_t size)
{
    if(NativeIsBigEndian())
    {
        uint16_t result;
        for(int i = 0; i < size; i++)
        {
            result = ((data[i] & 0x00FF) << 8) | ((data[i] & 0xFF00) >> 8);
            data[i] = result;
        }
    }
}

void swapEndiannessU32(uint32_t* data, uintptr_t size)
{
    if(NativeIsBigEndian())
    {
        uint32_t result;
        for(int i = 0; i < size; i++)
        {
            result = ((data[i] & 0x000000FF) << 24) | ((data[i] & 0x0000FF00) << 8) |
                     ((data[i] & 0x00FF0000) >> 8) | ((data[i] & 0xFF000000) >> 24);
            data[i] = result;
        }
    }
}

void swapEndiannessU64(uint64_t *data, uintptr_t size)
{
    if(NativeIsBigEndian())
    {
        uint64_t result;
        for(int i = 0; i < size; i++)
        {
            result = ((data[i] & 0x00000000000000FF) << 56) | ((data[i] & 0x000000000000FF00) << 40) |
                     ((data[i] & 0x0000000000FF0000) << 24) | ((data[i] & 0x00000000FF000000) << 8) |
                     ((data[i] & 0x000000FF00000000) >> 8) | ((data[i] & 0x0000FF0000000000) >> 24) |
                     ((data[i] & 0x00FF000000000000) >> 40) | ((data[i] & 0xFF00000000000000) >> 56);
                      data[i] = result;
        }
    }
}

}

}
