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
