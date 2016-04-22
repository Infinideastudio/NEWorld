#ifndef WORLDGEN_H
#define WORLDGEN_H

#include "RandGen.h"

class WorldGenerator
{
private:
    double m_perm[256];
    int m_seed;
    double m_NoiseScaleX, m_NoiseScaleZ;
    int m_WaterLevel;
    RandGen *rng;

    // 禁止无参数创建WorldGenerator实例
    WorldGenerator();

    void perlinNoiseInit(int mapseed);

    double Noise(int x, int y);

    double SmoothedNoise(int x, int y);

    double Interpolate(double a, double b, double x)
    {
        return a * (1.0 - x) + b * x;
    }

    double InterpolatedNoise(double x, double y);

    double PerlinNoise2D(double x, double y);

public:
    WorldGenerator(int mapseed);

    ~WorldGenerator();

    int getHeight(int x, int y)
    {
        return (int)PerlinNoise2D(x / m_NoiseScaleX + 0.125 , y / m_NoiseScaleZ + 0.125) >> 2;
    }

    int getWaterLevel()
    {
        return m_WaterLevel;
    }

};

#endif
