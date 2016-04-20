#ifndef RANDGEN_H
#define RANDGEN_H

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

#endif
