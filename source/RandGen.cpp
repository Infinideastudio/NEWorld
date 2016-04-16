<<<<<<< HEAD
/*
 * Originally written by Christian Stigen Larsen
 * http://csl.name
 *
 * This file is distributed under the modified BSD license.
 *
 * Modified and optimized by DLaboratory to adapt NEWorld project.
 */

#include "RandGen.h"

inline unsigned int M32(unsigned int x)
{
    return (0x80000000 & x);
}

inline unsigned int L31(unsigned int x)
{
    return (0x7FFFFFFF & x);
}

inline unsigned int ODD(unsigned int x)
{
    return (x & 1);
}

void RandGen::generate_numbers()
{
    const unsigned int MATRIX[2] = {0, 0x9908b0df};
    unsigned int y, i = 0, j;

    while ( i < (DIFF-1) )
    {
        y = M32(MT[i]) | L31(MT[i+1]);
        MT[i] = MT[i+PERIOD] ^ (y >> 1) ^ MATRIX[ODD(y)];
        ++i;
    }

    y = M32(MT[i]) | L31(MT[i+1]);
    MT[i] = MT[(i+PERIOD)%SIZE] ^ (y >> 1) ^ MATRIX[ODD(y)];
    ++i;

    while ( i < (SIZE-1) )
    {
        y = M32(MT[i]) | L31(MT[i+1]);
        MT[i] = MT[i-DIFF] ^ (y >> 1) ^ MATRIX[ODD(y)];
        ++i;
    }

    y = M32(MT[SIZE-1]) | L31(MT[0]);
    MT[SIZE-1] = MT[PERIOD-1] ^ (y>>1) ^ MATRIX[ODD(y)];
}

void RandGen::seed(unsigned int value)
{
    MT[0] = value;
    index = 0;

    for (unsigned int i = 1; i < SIZE; ++i)
        MT[i] = 0x6c078965 * (MT[i-1] ^ MT[i-1] >> 30) + i;
}

unsigned int RandGen::rand_u32()
{
    if ( !index )
        generate_numbers();

    unsigned int y = MT[index];

    y ^= y>>11;
    y ^= y<< 7 & 0x9d2c5680;
    y ^= y<<15 & 0xefc60000;
    y ^= y>>18;

    if ( ++index == SIZE )
        index = 0;

    return y;
}

unsigned long long RandGen::rand_u64()
{
    return (((unsigned long long)(rand_u32()) << 32) | (unsigned long long)rand_u32());
}

int RandGen::rand_s32()
{
    return static_cast<int>(0x7FFFFFFF & rand_u32());
}

float RandGen::randf_cc()
{
    return static_cast<float>(rand_u32()) / 0xFFFFFFFF;
}

float RandGen::randf_co()
{
    return static_cast<float>(rand_u32()) / (0xFFFFFFFF + 1.0f);
=======
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
>>>>>>> 0.5.0
}
