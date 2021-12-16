#pragma once
//Types/constants define

#include <cstdint>
#include <thread>
#include <mutex>

typedef unsigned char ubyte;
typedef unsigned long uint32;
typedef unsigned long long uint64;

typedef unsigned char blockprop;
typedef unsigned char Brightness;
typedef unsigned int TextureID;
typedef uint32_t item;
typedef unsigned int VBOID;
typedef int vtxCount;
typedef int SkinID;
typedef uint64 chunkid;
typedef unsigned int onlineid;
#ifdef NEWORLD_GAME
typedef std::mutex *Mutex_t;

typedef unsigned int(*ThreadFunc_t)(void *param);

#define ThreadFunc unsigned int
#endif

/*struct Block {
    uint16_t Id : 16;
    uint8_t BrightnessBlock : 4;
    uint8_t BrightnessSky : 4;
    uint8_t Meta0;
};*/

using Block = uint32_t;
