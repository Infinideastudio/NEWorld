#ifndef VEC3_H_
#define VEC3_H_

class Vec3
{
public:
    int x, y, z;

    Vec3::Vec3() :x(0), y(0), z(0) {}
    Vec3::Vec3(int x_, int y_, int z_) :x(x_), y(y_), z(z_) {}
    Vec3::Vec3(const Vec3& rhs) :x(rhs.x), y(rhs.y), z(rhs.z) {}

    void operator= (const Vec3& rhs)
    {
        x = rhs.x; y = rhs.y; z = rhs.z;
    }

    bool operator== (const Vec3& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    bool operator!= (const Vec3& rhs) const
    {
        return x != rhs.x || y != rhs.y || z != rhs.z;
    }

    Vec3 operator+ (const Vec3& rhs) const
    {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vec3 operator- (const Vec3& rhs) const
    {
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

};

#endif // !VEC3_H_
