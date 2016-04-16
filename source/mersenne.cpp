#include "mersenne.h"
#include <ctime>
using namespace std;

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
    seed(time(NULL));
}

MersenneRandGen::MersenneRandGen(unsigned int k)
{
    seed(k);
}

void MersenneRandGen::generate_numbers()
{
    unsigned int y, i = 0, j;

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
