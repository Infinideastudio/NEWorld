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

#ifndef VERTEXARRAY_H_
#define VERTEXARRAY_H_

#include <cstring>

class VertexArray
{
    private:
        // Vertex count
        int _vertexes, _maxVertexes;
        // Vertex Attribute Count
        int _textureCount, _colorCount, _attributeCount, _coordinateCount;
        // Vertex Attributes
        float *_vertexAttributes;
        // Attributes count
        int _vertexAttributeCount;
        // Array
        float *_data;

    public:
        VertexArray(int maxVertexes, int textureElementCount, int colorElementCount, int attributeElementCount, int coordinateElementCount)
            :_maxVertexes(maxVertexes), _vertexAttributeCount(textureElementCount + colorElementCount + attributeElementCount + coordinateElementCount),
            _textureCount(textureElementCount), _colorCount(colorElementCount), _attributeCount(attributeElementCount), _coordinateCount(coordinateElementCount)
        {
            _data = new float[_maxVertexes*_vertexAttributeCount];
            _vertexAttributes = new float[_vertexAttributeCount];
            clear();
        }
        
        ~VertexArray()
        {
            delete[] _data;
            delete[] _vertexAttributes;
        }
        
        void clear()
        {
            memset(_data, 0, _maxVertexes*_vertexAttributeCount*sizeof(float));
            memset(_vertexAttributes, 0, _vertexAttributeCount*sizeof(float));
            _vertexes = 0;
        }

        void setTexture(const int size, const float* texture)
        { memcpy(_vertexAttributes, texture, size*sizeof(float)); }

        void setColor(const int size, const float* color)
        { memcpy(_vertexAttributes + _textureCount, color, size*sizeof(float)); }

        void setAttribute(const int size, const float* attribute)
        { memcpy(_vertexAttributes + _textureCount + _colorCount, attribute, size*sizeof(float)); }

        void addVertex(const float* coords)
        {
            memcpy(_data + _vertexes*sizeof(float), _vertexAttributes, _vertexAttributeCount*sizeof(float));
            memcpy(_data + _vertexes + _vertexAttributeCount*sizeof(float), coords, _coordinateCount*sizeof(float));
            _vertexes++;
        }

        void flush()
        {

        }

};

#endif // !VERTEXARRAY_H_
