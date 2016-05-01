/*
 * NEWorld: An free game with similar rules to Minecraft.
 * Copyright (C) 2016 NEWorld Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "RandGen.h"
#include <algorithm>
#include <ctime>
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

// Mersenne Twister Random Number Generation Algorithm

inline unsigned int M32(unsigned int x)
{
    return (0x80000000 & x);
}

inline unsigned int L31(unsigned int x)
{
    return (0x7FFFFFFF & x);
}

inline unsigned int matrix(unsigned int x)
{
    return (x & 1) ? 0x9908b0df : 0;
}

MersenneRandGen::MersenneRandGen()
{
    seed((unsigned int)time(NULL));
}

MersenneRandGen::MersenneRandGen(unsigned int k)
{
    seed(k);
}

void MersenneRandGen::generate_numbers()
{
    unsigned int y, i = 0;

    while ( i < (diff-1) )
    {
        y = M32(buffer[i]) | L31(buffer[i+1]);
        buffer[i] = buffer[i+period] ^ (y >> 1) ^ matrix(y);
        ++i;
    }

    y = M32(buffer[i]) | L31(buffer[i+1]);
    buffer[i] = buffer[(i+period)%buffer_size] ^ (y >> 1) ^ matrix(y);
    ++i;

    while ( i < (buffer_size-1) )
    {
        y = M32(buffer[i]) | L31(buffer[i+1]);
        buffer[i] = buffer[i-diff] ^ (y >> 1) ^ matrix(y);
        ++i;
    }

    y = M32(buffer[buffer_size-1]) | L31(buffer[0]);
    buffer[buffer_size-1] = buffer[period-1] ^ (y>>1) ^ matrix(y);
}

void MersenneRandGen::seed(unsigned int k)
{
    buffer[0] = k;
    index = 0;

    for (unsigned int i = 1; i < buffer_size; ++i)
        buffer[i] = 0x6c078965 * (buffer[i-1] ^ buffer[i-1] >> 30) + i;
}

unsigned int MersenneRandGen::get_u32()
{
    if ( !index )
        generate_numbers();

    unsigned int y = buffer[index];

    y ^= y>>11;
    y ^= y<< 7 & 0x9d2c5680;
    y ^= y<<15 & 0xefc60000;
    y ^= y>>18;

    if (++index == buffer_size)
        index = 0;

    return y;
}

// Linear Recurrence Random Number Generation Algorithm

LinearRandGen::LinearRandGen()
{
    coefficient = 214013;
    offset = 2531011;
    seed((unsigned int)time(NULL));
}

LinearRandGen::LinearRandGen(unsigned int k)
{
    coefficient = 214013;
    offset = 2531011;
    seed(k);
}

LinearRandGen::LinearRandGen(unsigned int k, unsigned int _coefficient, unsigned int _offset)
{
    coefficient = _coefficient;
    offset = _offset;
    seed(k);
}

void LinearRandGen::seed(unsigned int k)
{
    v = k;
}

unsigned int LinearRandGen::get_u32()
{
    v = v * coefficient + offset;
    return v;
}

#if (defined NEWORLD_TARGET_MACOSX) && (defined __RDRND__)
unsigned int IntelRandGen::get_u32()
{
    _rdrand32_step(&value);
    return value;
}
#endif
