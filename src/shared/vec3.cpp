#include "vec3.h"

Vec3::Vec3() :x(0), y(0), z(0)
{
}

Vec3::Vec3(int x_, int y_, int z_) :x(x_), y(y_), z(z_)
{
}

Vec3 operator+(const Vec3 v1, const Vec3 v2)
{
    return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
