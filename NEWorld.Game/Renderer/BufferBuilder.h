#pragma once

#include <array>
#include <algorithm>
#include "Definitions.h"
#include "Temp/Vector.h"

namespace Renderer {
    enum class BufferType {
        Vertex,
        Index,
        Indirect
    };

    template<class B = float>
    class BufferBuilder {
        static constexpr ptrdiff_t size = 1024;
        static constexpr ptrdiff_t mask = size - 1;
        using Array = std::array<B, size>;
    public:
        template<int count, class T = float, class... Elem>
        void put(Elem... elem) noexcept {
            std::initializer_list<B> v{static_cast<B>(std::forward<Elem>(elem))...};
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
                if (buffer == 0) glCreateBuffers(1, &buffer);
                const auto segmentSize = static_cast<GLsizeiptr>(mSectors.size() * size * sizeof(B));
                glBindBuffer(GL_ARRAY_BUFFER, buffer);
                glNamedBufferStorage(buffer, segmentSize, nullptr, GL_MAP_WRITE_BIT);
                auto target = reinterpret_cast<B *>(glMapNamedBufferRange(
                        buffer, 0, segmentSize,
                        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
                ));
                for (auto &s: mSectors) target = std::copy(s->begin(), s->end(), target);
                glFlushMappedNamedBufferRange(buffer, 0, segmentSize);
                glUnmapNamedBuffer(buffer);
            }
        }

    private:
        void make_sector() noexcept { mSectors.emplace_back(temp::make_unique<Array>()); }

        temp::vector<temp::unique_ptr<Array>> mSectors{};
        ptrdiff_t mTail{0u}, mVts{0u};
    };
}