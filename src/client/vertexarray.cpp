/*
* NEWorld: A free game with similar rules to Minecraft.
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

#include "vertexarray.h"

VertexBuffer::VertexBuffer(const VertexArray& va) :vertexes(va.getVertexCount()), format(va.getFormat())
{
    glGenBuffersARB(1, &id);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, id);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, va.getVertexCount() * sizeof(float) *
                    format.vertexAttributeCount,
                    va.getData(), GL_STATIC_DRAW_ARB);
}

void VertexBuffer::render() const
{
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, id);
    if (format.textureCount != 0)
        glTexCoordPointer(
            format.textureCount, GL_FLOAT,
            format.vertexAttributeCount * sizeof(float),
            nullptr
        );
    if (format.colorCount != 0)
        glColorPointer(
            format.colorCount, GL_FLOAT,
            format.vertexAttributeCount * sizeof(float),
            reinterpret_cast<float*>(format.textureCount * sizeof(float))
        );
    if (format.normalCount != 0)
        glNormalPointer(
            /*format.normalCount,*/ GL_FLOAT,
            format.vertexAttributeCount * sizeof(float),
            reinterpret_cast<float*>((format.textureCount + format.colorCount) * sizeof(float))
        );
    if (format.coordinateCount != 0)
        glVertexPointer(
            format.coordinateCount, GL_FLOAT,
            format.vertexAttributeCount * sizeof(float),
            reinterpret_cast<float*>((format.textureCount + format.colorCount + format.normalCount) * sizeof(float))
        );

    // 这个框是不是很装逼2333 --qiaozhanrong
    //====================================================================================================//
    /**/                                                                                                /**/
    /**/                                                                                                /**/
    /**/                              glDrawArrays(GL_QUADS, 0, vertexes);                              /**/
    /**/                                                                                                /**/
    /**/                                                                                                /**/
    //====================================================================================================//
}
