#pragma once

#include "Definitions.h"
#include "Temp/Vector.h"
#include <algorithm>

namespace Renderer {
    enum class BufferType {
        Vertex,
        Index,
        Indirect
    };

    class VertexBufferBuilder {
        static constexpr ptrdiff_t size = 1024;
        static constexpr ptrdiff_t mask = size - 1;
    public:
        template<int count, class T = float, class... Elem>
        void put(Elem... elem) noexcept {
            std::initializer_list<float> v{static_cast<float>(std::forward<Elem>(elem))...};
            const auto off = mTail & mask;
            const auto remain = size - off;
            if (off == 0) make_sector();
            if (remain >= static_cast<ptrdiff_t>(v.size())) {
                std::copy(v.begin(), v.end(), mSectors.back()->begin() + off);
            } else {
                std::copy(v.begin(), v.begin() + remain, mSectors.back()->begin() + off);
                make_sector();
                std::copy(v.begin() + remain, v.end(), mSectors.back()->begin());
            }
            mTail += static_cast<ptrdiff_t>(v.size());
            mVts += count;
        }

        // TODO: this is a temporary method
        void flush(VBOID &buffer, vtxCount &vts) const noexcept {
            vts = static_cast<vtxCount>(mVts);
            if (mVts != 0) {
                if (buffer == 0) glGenBuffers(1, &buffer);
                const auto segmentSize = static_cast<GLsizeiptr>(mSectors.size() * size * sizeof(float));
                glBindBuffer(GL_ARRAY_BUFFER, buffer);
                glBufferData(GL_ARRAY_BUFFER, segmentSize, nullptr, GL_STATIC_DRAW);
                auto target = reinterpret_cast<float *>(glMapBufferRange(
                        GL_ARRAY_BUFFER, 0, segmentSize,
                        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
                ));
                for (auto &s: mSectors) target = std::copy(s->begin(), s->end(), target);
                glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, segmentSize);
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
        }

    private:
        void make_sector() noexcept { mSectors.emplace_back(temp::make_unique<std::array<float, size>>()); }

        temp::vector<temp::unique_ptr<std::array<float, size>>> mSectors{};
        ptrdiff_t mTail{0u}, mVts{0u};
    };
}