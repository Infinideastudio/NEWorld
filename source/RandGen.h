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

#ifndef RANDGEN_H
#define RANDGEN_H

#include <immintrin.h>
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

#if (defined NEWORLD_TARGET_MACOSX) && (defined __RDRND__)
// Intel的RNRAND硬件随机数生成器
class IntelRandGen : public RandGen
{
private:
    unsigned int value;

public:
    // dummy function
    void seed(unsigned int k) {}

    // generate an unsigned 32bit integer
    unsigned int get_u32();
};
#endif

#endif
