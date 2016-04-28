/*
 * NEWorld: An free game with similar rules to Minecraft.
 * Copyright (C) 2016 NEWorld Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Effect.h"
#include "Definitions.h"
#include <math.h>

namespace Effect
{

void readScreen(int x, int y, int w, int h, uint8_t* data)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void writeScreen(int x, int y, int w, int h, uint8_t* data)
{
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glRasterPos2i(x, y + h);
    glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void gray(int w, int h, uint8_t* src, uint8_t* dst)
{
    uintptr_t size = w * h * 3;
    for (uintptr_t i = 0; i < size; i += 3)
    {
        dst[i] = dst[i + 1] = dst[i + 2] = (uint8_t)(0.299f * src[i]
                                           + 0.587f * src[i + 1]
                                           + 0.114f * src[i + 2]);
    }
}

void blurGaussianX(int w, int h, uint8_t* src, uint8_t* dst, int rx)
{
    int size = rx * 2 + 1, i, j, dx, x, indexsrc, indexdst;
    float* mat = new float[size];
    float sigma = (rx - 1) * 0.3f + 0.8f, val, sum = 0.0f;
    sigma = -0.5f / (sigma * sigma);
    for (i = 0; i < size; i++)
    {
        x = i - rx;
        x = x * x;
        val = ::expf(sigma * x);
        mat[i] = val;
        sum += val;
    }
    sum = 1.0f / sum;
    for (i = 0; i < size; i++)
        mat[i] *= sum;
    for (i = 0; i < w * h * 3; i += 3)
        dst[i] = dst[i + 1] = dst[i + 2] = 0;
    for (i = 0; i < size; i++)
    {
        dx = i - rx;
        val = mat[i];
        for (j = 0; j < w; j++)
        {
            x = j + dx;
            if (x < 0)
                x = 0;
            if (x >= w)
                x = w - 1;
            for (int k = 0; k < h; k++)
            {
                indexsrc = (k * w + j) * 3;
                indexdst = (k * w + x) * 3;
                dst[indexdst] += (uint8_t)(val * src[indexsrc]);
                dst[indexdst + 1] += (uint8_t)(val * src[indexsrc + 1]);
                dst[indexdst + 2] += (uint8_t)(val * src[indexsrc + 2]);
            }
        }
    }
    delete []mat;
}

void blurGaussianY(int w, int h, uint8_t* src, uint8_t* dst, int ry)
{
    int size = ry * 2 + 1, i, j, k, dy, y, indexsrc, indexdst;
    float* mat = new float[size];
    float sigma = (ry - 1) * 0.3f + 0.8f, sum = 0.0f, val;
    sigma = -0.5f / (sigma * sigma);
    for (i = 0; i < size; i++)
    {
        y = i - ry;
        y = y * y;
        val = ::expf(sigma * y);
        mat[i] = val;
        sum += val;
    }
    sum = 1.0f / sum;
    for (i = 0; i < size; i++)
        mat[i] *= sum;

    for (i = 0; i < w * h * 3; i += 3)
        dst[i] = dst[i + 1] = dst[i + 2] = 0;

    for (i = 0; i < size; i++)
    {
        dy = i - ry;
        val = mat[i];
        for (j = 0; j < h; j++)
        {
            y = j + dy;
            if (y < 0)
                y = 0;
            if (y >= h)
                y = h - 1;
            for (k = 0; k < w; k++)
            {
                indexsrc = (j * w + k) * 3;
                indexdst = (y * w + k) * 3;
                dst[indexdst] += (uint8_t)(val * src[indexsrc]);
                dst[indexdst + 1] += (uint8_t)(val * src[indexsrc + 1]);
                dst[indexdst + 2] += (uint8_t)(val * src[indexsrc + 2]);
            }
        }
    }
    delete []mat;
}

}
