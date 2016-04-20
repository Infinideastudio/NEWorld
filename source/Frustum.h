#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Hitbox.h"

class Frustum
{
private:
    float frus[24], clip[16];
    float proj[16], modl[16];

public:
    //AABB with Float32 coords
    struct ChunkBox
    {
        float xmin, ymin, zmin;
        float xmax, ymax, zmax;
    };

    float* getProjMatrix()
    {
        return proj;
    }

    float* getModlMatrix()
    {
        return modl;
    }

    void LoadIdentity();

    void MultMatrixTo(float* sum, float* a, float* b)
    {
        sum[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12];
        sum[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
        sum[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
        sum[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];
        sum[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
        sum[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
        sum[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
        sum[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];
        sum[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
        sum[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
        sum[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
        sum[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];
        sum[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
        sum[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
        sum[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
        sum[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
    }

    void MultMatrix(float* a, float* b);
    void SetPerspective(float FOV, float aspect, float Znear, float Zfar);
    void SetOrtho(float left, float right, float top, float bottom, float Znear, float Zfar);
    void MultRotate(float angle, float x, float y, float z);
    void normalize(int side);
    void update();
    bool FrustumTest(const ChunkBox& aabb);
};

#endif
