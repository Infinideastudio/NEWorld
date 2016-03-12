#include "WorldGen.h"
#include "Definitions.h"

//Perlin Noise 2D
namespace WorldGen
{

double perm[256];
int seed;
double NoiseScaleX = 64;
double NoiseScaleZ = 64;
int WaterLevel = 30;

void perlinNoiseInit(int mapseed)
{
    fastSrand(mapseed);
    for (int i = 0; i < 256; i++)
    {
        perm[i] = rnd() * 256.0;
    }
    seed = mapseed;
}

double SmoothedNoise(int x, int y)
{
    double corners, sides, center;
    corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) + Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 8.0;
    sides = (Noise(x - 1, y) + Noise(x + 1, y) + Noise(x, y - 1) + Noise(x, y + 1)) / 4.0;
    center = Noise(x, y);
    return corners + sides + center;
}

double InterpolatedNoise(double x, double y)
{
    int int_X, int_Y;
    double fractional_X, fractional_Y, v1, v2, v3, v4, i1, i2;
    int_X = (int)floor(x); //不要问我为毛用floor，c++默认居然TM的是向零取整的。 //你不知道吗？
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

double PerlinNoise2D(double x, double y)
{
    double total = 0, frequency = 1, amplitude = 1;
    for (int i = 0; i <= 4; i++)
    {
        total += InterpolatedNoise(x*frequency, y*frequency)*amplitude;
        frequency *= 2.0;
        amplitude /= 2.0;
    }
    return total;
}

}
