#pragma once

#include <array>
#include <algorithm>
#include "Definitions.h"
#include <kls/temp/STL.h>
#include <kls/coroutine/Async.h>
#include <kls/coroutine/Operation.h>
#include "Dispatch.h"

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
        kls::coroutine::ValueAsync<void> flushAsync(VBOID& buffer, vtxCount& vts) noexcept {
            vts = static_cast<vtxCount>(mVts);
            if (mVts != 0) {
                if (buffer != 0) glDeleteBuffers(1, &buffer);
                glCreateBuffers(1, &buffer);
                const auto segmentSize = static_cast<GLsizeiptr>(mSectors.size() * size * sizeof(B));
                glNamedBufferStorage(buffer, segmentSize, nullptr, GL_MAP_WRITE_BIT);
                co_await CopyAndRelease(reinterpret_cast<B*>(glMapNamedBufferRange(
                    buffer, 0, segmentSize,
                    GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
                )));
                glFlushMappedNamedBufferRange(buffer, 0, segmentSize);
                glUnmapNamedBuffer(buffer);
            }
        }

        // TODO: this is a temporary method
        void flush(VBOID &buffer, vtxCount &vts) const noexcept {
            vts = static_cast<vtxCount>(mVts);
            if (mVts != 0) {
                if (buffer != 0) glDeleteBuffers(1, &buffer);
                glCreateBuffers(1, &buffer);
                const auto segmentSize = static_cast<GLsizeiptr>(mSectors.size() * size * sizeof(B));
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
        void make_sector() noexcept { mSectors.emplace_back(kls::temp::make_unique<Array>()); }

        kls::coroutine::ValueAsync<void> CopyAndRelease(B* target) {
            co_await kls::coroutine::SwitchTo(GetSessionDefault());
            for (auto& s : mSectors) target = std::copy(s->begin(), s->end(), target);
            mSectors.clear();
        }

        kls::temp::vector<kls::pmr::unique_ptr<Array>> mSectors{};
        ptrdiff_t mTail{0u}, mVts{0u};
    };
}