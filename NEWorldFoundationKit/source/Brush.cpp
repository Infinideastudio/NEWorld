#include "Brush.h"
namespace NWUIK
{
    void Brush::PaintArea(Rect Area)
    {

    }

    void Brush::PaintWithColorMask(Color4f Color)
    {

    }

    void SolidColorBrush::PaintArea(Rect Area)
    {

    }

    void SolidColorBrush::PaintBorder(Rect Area)
    {

    }

    SolidColorBrush::SolidColorBrush(Color4f _Color) :
        Color(_Color)
    {

    }
}
