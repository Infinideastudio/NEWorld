#pragma once
//Types/constants define
typedef unsigned char ubyte;
typedef signed char int8;
typedef short int16;
typedef long int32;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;
typedef unsigned char blockprop;
typedef unsigned char brightness;
typedef unsigned int TextureID;
typedef unsigned int VBOID;
typedef int vtxCount;
typedef int SkinID;
typedef uint64 chunkid;
typedef unsigned int onlineid;

struct block
{
    unsigned short ID;
    union
    {
        struct
        {
            unsigned int Oriention : 5;
            unsigned int Oct : 3;
            unsigned int Hex1 : 4;
            unsigned int Hex4 : 4;
        };
        struct
        {
            unsigned int Oriention : 5;
            unsigned int Oct : 3;
            unsigned int Data8 : 8;
        };
        struct
        {
            unsigned int Data81 : 8;
            unsigned int Data82 : 8;
        };
        unsigned short Data16;
    };
    block() {};
    block(unsigned short iID) :ID(iID) {}
    block(unsigned short iID, int iOriention) :ID(iID), Oriention(iOriention) {};
    bool operator == (const block i)
    {
        return i.ID == ID;
    }
    bool operator != (const block i)
    {
        return i.ID != ID;
    }
};

namespace Blocks {
	struct BUDDP
	{
		block* upd;
		block* slf;
		void* dudp;
		void* dslf;
		long long cx, cy, cz;
		BUDDP(block* _upd, block* _slf, void* _dudp, void* _dslf,
			long long _cx, long long _cy, long long _cz) :
			upd(_upd), slf(_slf), dudp(_dudp), dslf(_dslf), cx(_cx), cy(_cy), cz(_cz){};
	};

	struct TILDP
	{
		block* slf;
		void* dslf;
		long long cx, cy, cz;

		TILDP(block* _slf, void* _dslf, long long _cx, long long _cy, long long _cz) :
			slf(_slf), dslf(_dslf), cx(_cx), cy(_cy), cz(_cz) {};
	};
}
typedef block item;

#ifdef NEWORLD_GAME
#ifdef NEWORLD_USE_WINAPI
typedef HANDLE Mutex_t;
typedef HANDLE Thread_t;
typedef PTHREAD_START_ROUTINE ThreadFunc_t;
#define ThreadFunc DWORD WINAPI
#else
typedef std::mutex* Mutex_t;
typedef std::thread* Thread_t;
typedef unsigned int(*ThreadFunc_t)(void* param);
#define ThreadFunc unsigned int
#endif
#endif
