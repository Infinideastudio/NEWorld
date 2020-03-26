// 
// GUI: vertexarray.h
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

#pragma once

#include <cstring>
#include <initializer_list>
#include "opengl.h"
#include "Common/Debug.h"
#include "Common/Logger.h"
#include "Common/Utility.h"

class VertexFormat {
public:
    // Vertex attribute count
    size_t textureCount, colorCount, normalCount, coordinateCount;
    // Vertex attributes count (sum of all)
    int vertexAttributeCount;

    VertexFormat() : textureCount(0), colorCount(0), normalCount(0), coordinateCount(0), vertexAttributeCount(0) { }

    VertexFormat(int textureElementCount, int colorElementCount, int normalElementCount, int coordinateElementCount)
        : textureCount(textureElementCount), colorCount(colorElementCount), normalCount(normalElementCount),
          coordinateCount(coordinateElementCount),
          vertexAttributeCount(textureElementCount + colorElementCount + normalElementCount + coordinateElementCount) {
        Assert(textureCount <= 3);
        Assert(colorCount <= 4);
        Assert(normalCount == 0 || normalCount == 3);
        Assert(coordinateCount <= 4 && coordinateCount >= 1);
    }
};

class VertexArray {
public:
    VertexArray(int maxVertexes, const VertexFormat& format)
    try :
        mMaxVertexes(maxVertexes), mVertexes(0), mFormat(format),
        mData(new float[mMaxVertexes * format.vertexAttributeCount]),
        mVertexAttributes(new float[format.vertexAttributeCount]) { }
    catch (std::bad_alloc&) {
        warningstream << "Failed to create Vertex Array: Out of memory. "
            << (maxVertexes * format.vertexAttributeCount) * sizeof(float) / 1024
            << "KiB needed.";
    }

    ~VertexArray() {
        delete[] mData;
        delete[] mVertexAttributes;
    }

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept:
        mMaxVertexes(other.mMaxVertexes),
        mVertexes(other.mVertexes),
        mFormat(other.mFormat),
        mData(other.mData),
        mVertexAttributes(other.mVertexAttributes) {
        if (this == &other) return;
        other.mData = nullptr;
        other.mVertexAttributes = nullptr;
    }

    VertexArray& operator=(VertexArray&& other) noexcept {
        if (this == &other) return *this;
        mVertexes = other.mVertexes;
        mMaxVertexes = other.mMaxVertexes;
        mData = other.mData;
        mVertexAttributes = other.mVertexAttributes;
        mFormat = other.mFormat;
        other.mData = nullptr;
        other.mVertexAttributes = nullptr;
        return *this;
    }

    void clear() {
        memset(mData, 0, mMaxVertexes * mFormat.vertexAttributeCount * sizeof(float));
        memset(mVertexAttributes, 0, mFormat.vertexAttributeCount * sizeof(float));
        mVertexes = 0;
    }

    void addPrimitive(size_t size, std::initializer_list<float> d) {
        memcpy(mData + mVertexes * mFormat.vertexAttributeCount, d.begin(),
               size * mFormat.vertexAttributeCount * sizeof(float));
        mVertexes += size;
    }

    // Get current vertex format
    [[nodiscard]] const VertexFormat& getFormat() const { return mFormat; }

    // Get current vertex data
    [[nodiscard]] const float* getData() const {
        return mData;
    }

    // Get current vertex count
    [[nodiscard]] size_t getVertexCount() const { return mVertexes; }

private:
    // Max vertex count
    size_t mMaxVertexes;
    // Vertex count
    size_t mVertexes;
    // Vertex array format
    VertexFormat mFormat;
    // Vertex array
    float* mData;
    // Current vertex attributes
    float* mVertexAttributes;
};

class VertexBuffer : public NonCopyable {
public:
    VertexBuffer(): id(0), vertexes(0) { }

    VertexBuffer(VertexBuffer&& rhs) noexcept:
        id(rhs.id), vertexes(rhs.vertexes), format(rhs.format) {
        rhs.vertexes = rhs.id = 0;
        rhs.format = VertexFormat();
    }

    explicit VertexBuffer(const VertexArray& va);

    VertexBuffer& operator=(VertexBuffer&& rhs) noexcept {
        id = rhs.id;
        vertexes = rhs.vertexes;
        format = rhs.format;
        rhs.vertexes = rhs.id = 0;
        rhs.format = VertexFormat();
        return *this;
    }

    ~VertexBuffer() { destroy(); }

    // upload new data
    void update(const VertexArray& va);

    // Render vertex buffer
    void render() const;

    // Destroy vertex buffer
    void destroy() {
        if (id) {
            glDeleteBuffers(1, &id);
            vertexes = id = 0;
            format = VertexFormat();
        }
    }

    [[nodiscard]] bool isEmpty() const noexcept { return (vertexes == 0); }

private:
    // Buffer ID
    VertexBufferID id;
    // Vertex count
    int vertexes;
    // Buffer format
    VertexFormat format;

};
