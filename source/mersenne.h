#ifndef MERSENNE_H
#define MERSENNE_H

#include "randgen.h"
class MersenneRandGen : public RandGen
{
private:
    static const unsigned int buffer_size = 624, period = 397, diff = 227;
    unsigned int buffer[buffer_size], index;

    void generate_numbers();

public:
    // automatically set seed to current UNIX time stamp
    MersenneRandGen();

    // set seed to the given value k
    MersenneRandGen(unsigned int k);

    // set seed to k
    void seed(unsigned int k);

    // generate an unsigned 32bit integer
    unsigned int get_u32();
};

#endif
