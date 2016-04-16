<<<<<<< HEAD
/*
 * Originally written by Christian Stigen Larsen
 * http://csl.name
 *
 * This file is distributed under the modified BSD license.
 *
 * Modified and optimized by DLaboratory to adapt NEWorld project.
 */

#ifndef _RANDGEN_H_
#define _RANDGEN_H_

class RandGen
{
private:
    static const unsigned SIZE = 624, PERIOD = 397, DIFF = 227;
    unsigned int MT[SIZE];
    unsigned int index;

    void generate_numbers();

public:
    //Extract a pseudo-random integer in the range 0 ... RAND_MAX.
    int rand_s32();

    //Extract a pseudo-random unsigned 32-bit integer in the range 0 ... UINT32_MAX
    unsigned int rand_u32();

    //Combine two unsigned 32-bit pseudo-random numbers into one 64-bit
    unsigned long long rand_u64();

    //Initialize Mersenne Twister with given seed value.
    void seed(unsigned int seed_value);

    //Return a random float in the CLOSED range [0, 1]
    float randf_cc();

    // Return a random float in the OPEN range [0, 1)
    float randf_co();
};

#endif
=======
#pragma once
class RandGen
{
public:
    virtual ~RandGen() {}
    virtual void seed(unsigned int k) = 0;

    // 64bit
    unsigned long long get_u64();
    long long get_s64();
    long long get_s64_ranged(long long x, long long y); // [x, y)

    // 32bit
    virtual unsigned int get_u32() = 0;
    int get_s32();
    int get_s32_ranged(int x, int y); // [x, y)

    // 16bit
    short get_s16();
    unsigned short get_u16();

    // Boolean
    bool one_in(int x); // returns true in possibility of 1/x
    bool x_in_y(int x, int y); // returns true in possibility of x/y

    // Float-point
    double get_double_co(); // returns double value in [0, 1)
    double get_double_cc(); // returns double value in [0, 1]
    double get_double_ranged(double x, double y); // [x, y)
};
>>>>>>> 0.5.0
