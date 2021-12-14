#include <new>
#include <atomic>
#include "Temp.h"

namespace {
    struct header final {
        std::atomic_int32_t flying{0};
    };

    header *fetch() noexcept { return new(internal::rent_block()) header; }

    void release(header *const blk) noexcept { internal::return_block(blk); }

    [[nodiscard]] constexpr uintptr_t max_align(const uintptr_t size) noexcept {
        constexpr auto mask = alignof(std::max_align_t) - 1;
        constexpr auto rev = ~mask;
        if (size & mask) return (size & rev) + alignof(std::max_align_t); else return size;
    }

    class allocation final {
        static constexpr auto alloc_start = max_align(sizeof(header));
        header *current{};
        uintptr_t head{};
        int32_t count{};
    public:
        [[nodiscard]] bool flush(header *&out) const noexcept {
            if (current) {
                out = current;
                return (current->flying.fetch_add(count) == -count);
            }
            return false;
        }

        void reset(header *const other) noexcept {
            current = other, head = alloc_start, count = 0;
        }

        [[nodiscard]] void *allocate(const uintptr_t size) noexcept {
            const auto aligned = max_align(size);
            if (const auto expected = head + aligned; expected < internal::block_size) {
                ++count;
                const auto res = reinterpret_cast<uintptr_t>(current) + head;
                head = expected;
                return reinterpret_cast<void *>(res);
            }
            return nullptr;
        }
    };
}

namespace internal {
    [[nodiscard]] void *temp_allocate(const uintptr_t size) noexcept {
        struct local final {
            allocation alloc{};

            local() noexcept { reset(); }

            ~local() noexcept { reset(nullptr); }

            void reset(header *const next = fetch()) noexcept {
                if (header *last = nullptr; alloc.flush(last)) {
                    if (last) release(last);
                }
                alloc.reset(next);
            }
        };
        static const thread_local auto o = std::make_unique<local>();
        for (;;) if (const auto ret = o->alloc.allocate(size); ret) return ret; else o->reset();
    }

    void temp_free(void *const mem) noexcept {
        static constexpr uintptr_t rev = 0b11'1111'1111'1111'1111'1111;
        static constexpr uintptr_t mask = ~rev;
        if (mem == nullptr) return;
        const auto base = reinterpret_cast<uintptr_t>(mem) & mask;
        const auto header = reinterpret_cast<struct header *>(base);
        if (header->flying.fetch_sub(1) == 1) release(header);
    }
}
