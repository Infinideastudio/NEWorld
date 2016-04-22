#include "WorldGen.h"
#include "Definitions.h"

WorldGenerator::WorldGenerator(int mapseed)
{
    RandGen *mersenne_gen = new MersenneRandGen(mapseed);
    perlinNoiseInit(mapseed);
    m_seed = mapseed;
    for (int i = 0; i < 256; i++)
        m_perm[i] = rng->get_double_ranged(0.0, 256.0);
}

WorldGenerator::~WorldGenerator()
{
    delete rng;
}

double WorldGenerator::Noise(int x, int y)
{
    long long xx = x + y * 13258953287;
    xx = (xx >> 13) ^ xx;
    return ((xx*(xx*xx * 15731 + 789221) + 1376312589) & 0x7fffffff) / 16777216.0;
}

double WorldGenerator::SmoothedNoise(int x, int y)
{
    double corners, sides, center;
    corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) + Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 8.0;
    sides = (Noise(x - 1, y) + Noise(x + 1, y) + Noise(x, y - 1) + Noise(x, y + 1)) / 4.0;
    center = Noise(x, y);
    return corners + sides + center;
}

double WorldGenerator::InterpolatedNoise(double x, double y)
{
    int int_X, int_Y;
    double fractional_X, fractional_Y, v1, v2, v3, v4, i1, i2;
    int_X = (int)floor(x);
    fractional_X = x - int_X;
    int_Y = (int)floor(y);
    fractional_Y = y - int_Y;
    v1 = Noise(int_X, int_Y);
    v2 = Noise(int_X + 1, int_Y);
    v3 = Noise(int_X, int_Y + 1);
    v4 = Noise(int_X + 1, int_Y + 1);
    i1 = Interpolate(v1, v2, fractional_X);
    i2 = Interpolate(v3, v4, fractional_X);
    return Interpolate(i1, i2, fractional_Y);
}

double WorldGenerator::PerlinNoise2D(double x, double y)
{
    double total = 0.0, frequency = 1.0, amplitude = 1.0;
    for (int i = 0; i <= 4; i++)
    {
        total += InterpolatedNoise(x * frequency, y * frequency) * amplitude;
        frequency *= 2.0;
        amplitude /= 2.0;
    }
    return total;
}

