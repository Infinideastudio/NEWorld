#include "RandGen.h"
#include <algorithm>
using namespace std;

unsigned long long RandGen::get_u64()
{
    unsigned long long low = get_u32(), high = get_u32();
    return (low << 32) + high;
}

long long RandGen::get_s64()
{
    return static_cast<long long>(0x7FFFFFFFFFFFFFFFULL & get_u64());
}

long long RandGen::get_s64_ranged(long long x, long long y)
{
    if(x > y)
        swap(x, y);
    return x + get_s64() % (y - x);
}

int RandGen::get_s32()
{
    return static_cast<int>(0x7FFFFFFF & get_u32());
}

int RandGen::get_s32_ranged(int x, int y)
{
    if(x > y)
        swap(x, y);
    return x + get_s32() % (y - x);
}

short RandGen::get_s16()
{
    return static_cast<short>(0x7FFF & get_u16());
}

unsigned short RandGen::get_u16()
{
    return get_u32() & 0xFFFF;
}

bool RandGen::one_in(int x)
{
    return x_in_y(1, x);
}

bool RandGen::x_in_y(int x, int y)
{
    return get_s32_ranged(0, y) < x;
}

double RandGen::get_double_co()
{
    return static_cast<double>(get_u32()) / (0xFFFFFFFF + 1.0f);
}

double RandGen::get_double_cc()
{
    return static_cast<double>(get_u32()) / 0xFFFFFFFF;
}

double RandGen::get_double_ranged(double x, double y)
{
    if(x > y)
        swap(x, y);
    return x + (y - x) * get_double_co();
}
