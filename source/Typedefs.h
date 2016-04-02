#pragma once
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
            unsigned int Hex2 : 4;
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
    block()
        :ID(0), Data16(0) {};
    block(unsigned short iID)
        :ID(iID), Data16(0) {}
    block(unsigned short iID, int iOriention)
        :ID(iID), Oriention(iOriention), Oct(0), Data8(0) {};
    block(unsigned short iID, int iData81, int iData82)
        :ID(iID), Data81(iData81), Data82(iData82) {};
    block(unsigned short iID, int iOriention, int iOct, int iData8)
        :ID(iID), Oriention(iOriention), Oct(iOct), Data8(iData8) {};
    block(unsigned short iID, int iOriention, int iOct, int iHex1, int iHex2)
        :ID(iID), Oriention(iOriention), Oct(iOct), Hex1(iHex1), Hex2(iHex2) {};

    bool operator == (const block i)
    {
        return i.ID == ID;
    }
    bool operator != (const block i)
    {
        return i.ID != ID;
    }
};

namespace Blocks
{
	struct BUDDP
	{
		block origon;
		block* upd;
		block* slf;
		void* dudp;
		void* dslf;
		long long cx, cy, cz;
		BUDDP(block iOri, block* _upd, block* _slf, void* _dudp, void* _dslf,
			  long long _cx, long long _cy, long long _cz) :
			origon(iOri), upd(_upd), slf(_slf), dudp(_dudp), dslf(_dslf), cx(_cx), cy(_cy), cz(_cz) {};
		bool operator == (const BUDDP i) {
			return cx == i.cx && cy == i.cy && cz == i.cz;
		}
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

class LogItem
{
public:
	enum LogLevel
	{
		Level_Information,
		Level_Warning,
		Level_Error
	};
	LogLevel Level;
	std::string Information;
	LogItem()
	{
	}
	LogItem(LogLevel _level, std::string _info)
		:Level(_level), Information(_info)
	{
	}
	std::string ToString()
	{
		return std::string("Level:") + (Level == Level_Information ? "Information" : Level == Level_Warning ? "Warning" : Level == Level_Error ? "Error" : "Unknown")
			+ ",Information:" + Information;
	}
};
