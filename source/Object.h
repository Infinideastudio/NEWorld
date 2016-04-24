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
