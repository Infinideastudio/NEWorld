#pragma once
#include "randgen.h"
class LinearRandGen : public RandGen
{
private:
    unsigned int coefficient, offset, v;

public:
    // automatically set seed to current UNIX time stamp
    LinearRandGen();
    
    // set seed to the given value k
    LinearRandGen(unsigned int k);

    // set seed, coefficient and offset manually
    LinearRandGen(unsigned int k, unsigned int _coefficient, unsigned int _offset);

    // set seed to k
    void seed(unsigned int k);

    // generate an unsigned 32bit integer
    unsigned int get_u32();
};
