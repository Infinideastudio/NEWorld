#ifndef OBJECT_H
#define OBJECT_H

#include "Definitions.h"

//Object 所有生物、可移动物体、非静态方块的基类
class Object
{
public:
    Object(double x, double y, double z) : m_x(x), m_y(y), m_z(z), VBO(0), vtxs(0) {};
    virtual ~Object() {};
    virtual void render() const = 0;
    const double getXPos() const
    {
        return m_x;
    }
    const double getYPos() const
    {
        return m_y;
    }
    const double getZPos() const
    {
        return m_z;
    }

protected:
    int m_id;
    double m_x, m_y, m_z;  //位置
    vtxCount vtxs;
    VBOID VBO;
};

#endif
