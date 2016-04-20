#ifndef EFFECT_H
#define EFFECT_H

#include "Definitions.h"

namespace Effect
{

void readScreen(int x, int y, int w, int h, uint8_t* data);
void writeScreen(int x, int y, int w, int h, uint8_t* data);

void gray(int w, int h, uint8_t* src, uint8_t* dst);
void blurGaussianX(int w, int h, uint8_t* src, uint8_t* dst, int rx);
void blurGaussianY(int w, int h, uint8_t* src, uint8_t* dst, int ry);

}

#endif
