#include "Frustum.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>

void Frustum::LoadIdentity()
{
    memset(proj, 0, sizeof(proj));
    memset(modl, 0, sizeof(modl));
    modl[0] = modl[5] = modl[10] = modl[15] = 1.0f;
}

inline void Frustum::MultMatrix(float * a, float * b)
{
    float sum[16];
    MultMatrixTo(sum, a, b);
    memcpy(a, sum, sizeof(sum));
}

void Frustum::SetPerspective(float FOV, float aspect, float Znear, float Zfar)
{
    float ViewAngleH = FOV * (float)M_PI / 180.0f;
    float ViewAngleV = atan(tan(ViewAngleH / 2.0f) * aspect) * 2.0f;
    proj[0] = 1.0f / tan(ViewAngleV / 2);
    proj[5] = proj[0] * aspect;
    proj[10] = -(Zfar + Znear) / (Zfar - Znear);
    proj[11] = -1;
    proj[14] = -2 * Zfar * Znear / (Zfar - Znear);
}

void Frustum::SetOrtho(float left, float right, float top, float bottom, float Znear, float Zfar)
{
    proj[0] = 2 / (right - left);
    proj[5] = 2 / (bottom - top);
    proj[10] = 2 / (Znear - Zfar);
    proj[15] = 1.0f;
}

void Frustum::MultRotate(float angle, float x, float y, float z)
{
    float m[16], sum[16];
    memset(m, 0, sizeof(m));
    float length = sqrtf(x * x + y * y + z * z);
    x /= length;
    y /= length;
    z /= length;
    float alpha = angle * (float)M_PI / 180.0f;
    float s = sin(alpha);
    float c = cos(alpha);
    float t = 1.0f - c;
    m[0] = t * x * x + c;
    m[1] = t * x * y + s * z;
    m[2] = t * x * z - s * y;
    m[4] = t * x * y - s * z;
    m[5] = t * y * y + c;
    m[6] = t * y * z + s * x;
    m[8] = t * x * z + s * y;
    m[9] = t * y * z - s * x;
    m[10] = t * z * z + c;
    m[15] = 1.0f;
    MultMatrixTo(sum, m, modl);
    memcpy(modl, sum, sizeof(sum));
}

inline void Frustum::normalize(int side)
{
    float magnitude = sqrtf(frus[side + 0] * frus[side + 0] + frus[side + 1] * frus[side + 1] + frus[side + 2] * frus[side + 2]);
    frus[side + 0] /= magnitude;
    frus[side + 1] /= magnitude;
    frus[side + 2] /= magnitude;
    frus[side + 3] /= magnitude;
}

void Frustum::update()
{
    MultMatrixTo(clip, modl, proj);

    frus[0] = clip[3] - clip[0];
    frus[1] = clip[7] - clip[4];
    frus[2] = clip[11] - clip[8];
    frus[3] = clip[15] - clip[12];
    normalize(0);

    frus[4] = clip[3] + clip[0];
    frus[5] = clip[7] + clip[4];
    frus[6] = clip[11] + clip[8];
    frus[7] = clip[15] + clip[12];
    normalize(4);

    frus[8] = clip[3] + clip[1];
    frus[9] = clip[7] + clip[5];
    frus[10] = clip[11] + clip[9];
    frus[11] = clip[15] + clip[13];
    normalize(8);

    frus[12] = clip[3] - clip[1];
    frus[13] = clip[7] - clip[5];
    frus[14] = clip[11] - clip[9];
    frus[15] = clip[15] - clip[13];
    normalize(12);

    frus[16] = clip[3] - clip[2];
    frus[17] = clip[7] - clip[6];
    frus[18] = clip[11] - clip[10];
    frus[19] = clip[15] - clip[14];
    normalize(16);

    frus[20] = clip[3] + clip[2];
    frus[21] = clip[7] + clip[6];
    frus[22] = clip[11] + clip[10];
    frus[23] = clip[15] + clip[14];
    normalize(20);
}

bool Frustum::FrustumTest(const ChunkBox& aabb)
{
    for (int i = 0; i < 24; i += 4)
    {
        if (frus[i] * aabb.xmin + frus[i + 1] * aabb.ymin + frus[i + 2] * aabb.zmin + frus[i + 3] <= 0.0f &&
                frus[i] * aabb.xmax + frus[i + 1] * aabb.ymin + frus[i + 2] * aabb.zmin + frus[i + 3] <= 0.0f &&
                frus[i] * aabb.xmin + frus[i + 1] * aabb.ymax + frus[i + 2] * aabb.zmin + frus[i + 3] <= 0.0f &&
                frus[i] * aabb.xmax + frus[i + 1] * aabb.ymax + frus[i + 2] * aabb.zmin + frus[i + 3] <= 0.0f &&
                frus[i] * aabb.xmin + frus[i + 1] * aabb.ymin + frus[i + 2] * aabb.zmax + frus[i + 3] <= 0.0f &&
                frus[i] * aabb.xmax + frus[i + 1] * aabb.ymin + frus[i + 2] * aabb.zmax + frus[i + 3] <= 0.0f &&
                frus[i] * aabb.xmin + frus[i + 1] * aabb.ymax + frus[i + 2] * aabb.zmax + frus[i + 3] <= 0.0f &&
                frus[i] * aabb.xmax + frus[i + 1] * aabb.ymax + frus[i + 2] * aabb.zmax + frus[i + 3] <= 0.0f)
        {
            return false;
        }
    }
    return true;
}