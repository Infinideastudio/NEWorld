// 
// GUI: vertexarray.cpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#include "vertexarray.h"

void VertexBuffer::update(const VertexArray& va) {
    vertexes = va.getVertexCount();
    format = va.getFormat();
    if (vertexes == 0)
        destroy();
    else {
        if (id == 0)
            glGenBuffersARB(1, &id);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, id);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, va.getVertexCount() * sizeof(float) *
                        format.vertexAttributeCount, va.getData(), GL_STATIC_DRAW_ARB);
    }
}

VertexBuffer::VertexBuffer(const VertexArray& va)
    : VertexBuffer() { update(va); }

void VertexBuffer::render() const {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, id);
    if (format.textureCount != 0) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(
            format.textureCount, GL_FLOAT,
            format.vertexAttributeCount * sizeof(float),
            nullptr
        );
    }
    if (format.colorCount != 0) {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(
            format.colorCount, GL_FLOAT,
            format.vertexAttributeCount * sizeof(float),
            reinterpret_cast<float*>(format.textureCount * sizeof(float))
        );
    }
    if (format.normalCount != 0) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(
            /*format.normalCount,*/ GL_FLOAT,
                                    format.vertexAttributeCount * sizeof(float),
                                    reinterpret_cast<float*>((format.textureCount + format.colorCount) * sizeof(float))
        );
    }
    if (format.coordinateCount != 0) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(
            format.coordinateCount, GL_FLOAT,
            format.vertexAttributeCount * sizeof(float),
            reinterpret_cast<float*>((format.textureCount + format.colorCount + format.normalCount) * sizeof(float))
        );
    }

    glDrawArrays(GL_QUADS, 0, vertexes);

    if (format.textureCount != 0)
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    if (format.colorCount != 0)
        glDisableClientState(GL_COLOR_ARRAY);
    if (format.normalCount != 0)
        glDisableClientState(GL_NORMAL_ARRAY);
    if (format.coordinateCount != 0)
        glDisableClientState(GL_VERTEX_ARRAY);

}
