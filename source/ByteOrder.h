#pragma once
#include <stdint.h>

namespace NNN
{

namespace ByteOrder
{
//Convert byte order
template <typename Type>
void convLENative(Type* data, uintptr_t count)
{
    int bytecount = sizeof(Type);
    for (uint32_t i = 0; i < count; i++)
    {
        uint8_t* cur = (uint8_t*)data + i*bytecount;
        Type sum = 0;
        for (int32_t j = 0; j < bytecount; j++)
        {
            sum ^= (Type)cur[j] << (8 * j);
        }
        *(Type*)cur = sum;
    }
}

template <typename Type>
void convNativeLE(Type* data, uintptr_t count)
{
    int bytecount = sizeof(Type);
    for (uint32_t i = 0; i < count; i++)
    {
        uint8_t* cur = (uint8_t*)data + i*bytecount;
        Type sum = *(Type*)cur;
        for (int32_t j = 0; j < bytecount; j++)
        {
            cur[j] = sum & ((Type)0xff << (8 * j));
        }
    }
}

inline void convertLE2NativeU16(uint16_t* data, uintptr_t size)
{
    convLENative(data, size);
}
inline void convertLE2NativeU32(uint32_t* data, uintptr_t size)
{
    convLENative(data, size);
}
inline void convertLE2NativeU64(uint64_t* data, uintptr_t size)
{
    convLENative(data, size);
}
inline void convertLE2NativeS16(int16_t* data, uintptr_t size)
{
    convLENative((uint16_t*)data, size);
}
inline void convertLE2NativeS32(int32_t* data, uintptr_t size)
{
    convLENative((uint32_t*)data, size);
}
inline void convertLE2NativeS64(int64_t* data, uintptr_t size)
{
    convLENative((uint64_t*)data, size);
}
inline void convertLE2NativeF32(float* data, uintptr_t size)
{
    convLENative((uint32_t*)data, size);
}
inline void convertLE2NativeF64(double* data, uintptr_t size)
{
    convLENative((uint64_t*)data, size);
}

inline void convertNative2LEU16(uint16_t* data, uintptr_t size)
{
    convNativeLE(data, size);
}
inline void convertNative2LEU32(uint32_t* data, uintptr_t size)
{
    convNativeLE(data, size);
}
inline void convertNative2LEU64(uint64_t* data, uintptr_t size)
{
    convNativeLE(data, size);
}
inline void convertNative2LES16(int16_t* data, uintptr_t size)
{
    convNativeLE((uint16_t*)data, size);
}
inline void convertNative2LES32(int32_t* data, uintptr_t size)
{
    convNativeLE((uint32_t*)data, size);
}
inline void convertNative2LES64(int64_t* data, uintptr_t size)
{
    convNativeLE((uint64_t*)data, size);
}
inline void convertNative2LEF32(float* data, uintptr_t size)
{
    convNativeLE((uint32_t*)data, size);
}
inline void convertNative2LEF64(double* data, uintptr_t size)
{
    convNativeLE((uint64_t*)data, size);
}
}

}
