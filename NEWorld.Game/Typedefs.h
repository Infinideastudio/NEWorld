#pragma once
//Types/constants define

#include <cstdint>
#include <thread>
#include <mutex>

typedef unsigned char ubyte;

typedef unsigned char blockprop;
typedef unsigned char Brightness;
typedef unsigned int TextureID;
typedef uint32_t item;
typedef unsigned int VBOID;
typedef int vtxCount;
typedef uint64_t chunkid;
typedef unsigned int onlineid;
#ifdef NEWORLD_GAME
typedef std::mutex *Mutex_t;

#endif

using Block = uint32_t;
