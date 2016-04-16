#include "linearrand.h"
#include <ctime>
using namespace std;

LinearRandGen::LinearRandGen()
{
    coefficient = 214013;
    offset = 2531011;
    seed(time(NULL));
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
