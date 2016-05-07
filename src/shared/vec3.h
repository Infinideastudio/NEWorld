#ifndef VEC3_H_
#define VEC3_H_

class Vec3
{
    public:
        int x, y, z;

        Vec3();
        Vec3(int x, int y, int z);

        friend Vec3 operator+ (const Vec3 v1, const Vec3 v2);

};

#endif // !VEC3_H_
