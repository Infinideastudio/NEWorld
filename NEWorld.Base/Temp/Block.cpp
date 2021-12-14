#include <mutex>
#include "Internal/system.h"
#include "Temp.h"

namespace {
    class block_host {
        // Sys Mem Management operations
        static auto reserve() noexcept {
            return VirtualAlloc(nullptr, g_reserved_address_space, MEM_RESERVE, PAGE_READWRITE);
        }

        void commit(const uint32_t block) const noexcept {
            VirtualAlloc(reinterpret_cast<LPVOID>(compute_base(block)), g_block_size, MEM_COMMIT, PAGE_READWRITE);
        }

        void release(const uint32_t block) const noexcept {
            VirtualFree(reinterpret_cast<LPVOID>(compute_base(block)), g_block_size, MEM_DECOMMIT);
        }

    public:
        block_host() noexcept
                : m_base_address(reinterpret_cast<uintptr_t>(reserve())),
                  m_start_address(block_align(m_base_address)), m_brk(0u), m_alloc(0u) {
        }

        // we do not need to cleanup anything as the OS will release them all on process termination

        void* allocate() noexcept {
            const std::lock_guard lock(m_lock);
            return reinterpret_cast<void*>(compute_base(alloc_id()));
        }

        void free(void* const ptr) noexcept {
            const std::lock_guard lock(m_lock);
            release_id((reinterpret_cast<uintptr_t>(ptr) - m_start_address) >> g_block_size_shl);
        }

        static block_host &instance() noexcept {
            static block_host instance{};
            return instance;
        }

    private:
        const uintptr_t m_base_address;
        const uintptr_t m_start_address;
        uint32_t m_brk, m_alloc;
        std::mutex m_lock;
        static constexpr uintptr_t g_block_size_shl = 22ull;
        static constexpr uintptr_t g_block_size = 1ull << g_block_size_shl;
        static constexpr uintptr_t g_reserved_address_space = 4ull << 40ull;

        // basic alignment computation
        [[nodiscard]] uintptr_t compute_base(const uint32_t block) const noexcept {
            return m_start_address + (block << g_block_size_shl);
        }

        static uintptr_t block_align(const uintptr_t in) noexcept {
            constexpr auto mask = (g_block_size - 1);
            return (in + mask) & (~mask);
        }

        //AVL tree temp_free-space management
        struct avl_node {
            avl_node *left;
            avl_node *right;
            avl_node *parent;
            intptr_t height;

            [[nodiscard]] uintptr_t key() const noexcept { return reinterpret_cast<uintptr_t>(this); }

            [[nodiscard]] uintptr_t value() const noexcept { return key(); }

            [[nodiscard]] intptr_t left_height() const noexcept { return (left ? left->height : 0u); }

            [[nodiscard]] intptr_t right_height() const noexcept { return (right ? right->height : 0u); }

            [[nodiscard]] auto heights() const noexcept { return std::pair(left_height(), right_height()); }

            auto &select(const bool is_left) noexcept { if (is_left) return left; else return right; }

            void set_left(avl_node *const node) noexcept { if ((left = node)) node->parent = this; }

            void set_right(avl_node *const node) noexcept { if ((right = node)) node->parent = this; }

            void replace(avl_node *const find, avl_node *const value) noexcept {
                if (left == find) set_left(value); else if (right == find) set_right(value);
            }

            bool fix_height() noexcept {
                if (const auto new_height = std::max(left_height(), right_height()) + 1; height != new_height)
                    return (height = new_height, true);
                else return false;
            }

            auto reset(avl_node *const np) noexcept { return (left = right = nullptr, parent = np, height = 1, this); }
        };

        class avl_tree {
        public:
            void add(avl_node *const node) noexcept {
                if (!root) return (min = max = root = node->reset(nullptr), void());
                const auto n_key = node->key();
                for (auto current = root;;) {
                    const auto a_key = current->key();
                    const auto target_left = n_key < a_key;
                    if (auto &target = current->select(target_left); !target) {
                        target = node->reset(current);
                        if (target_left && current == min) min = node;
                        if (!target_left && current == max) max = node;
                        node_fix_up(current);
                        break;
                    } else current = target;
                }
            }

            [[nodiscard]] bool remove_max_if(const uintptr_t v) noexcept {
                if (max == nullptr) return false;
                if (max->value() != v) return false;
                delete_leaf(max);
                return true;
            }

            [[nodiscard]] uintptr_t extract_min() noexcept {
                if (min == nullptr) return 0ull;
                const auto ret = min->value();
                delete_leaf(min);
                return ret;
            }

        private:
            avl_node *root = nullptr;
            avl_node *min = nullptr;
            avl_node *max = nullptr;

            bool single_rotate_left(avl_node *const n_left) noexcept {
                const auto parent = n_left->parent;
                const auto n_root = n_left->right;
                const auto shifted = n_root->left;
                n_root->set_left(n_left);
                n_left->set_right(shifted);
                if (parent) parent->replace(n_left, n_root); else (root = n_root, n_root->parent = nullptr);
                n_left->fix_height();
                return n_root->fix_height();
            }

            bool single_rotate_right(avl_node *const n_right) noexcept {
                const auto parent = n_right->parent;
                const auto n_root = n_right->left;
                const auto shifted = n_root->right;
                n_root->set_right(n_right);
                n_right->set_left(shifted);
                if (parent) parent->replace(n_right, n_root); else (root = n_root, n_root->parent = nullptr);
                n_right->fix_height();
                return n_root->fix_height();
            }

            bool double_rotate_left_right(avl_node *const avl) noexcept {
                single_rotate_left(avl->left);
                return single_rotate_right(avl);
            }

            bool double_rotate_right_left(avl_node *const avl) noexcept {
                single_rotate_right(avl->right);
                return single_rotate_left(avl);
            }

            bool try_balance(avl_node *const avl) noexcept {
                const auto[lh, rh] = avl->heights();
                if (lh - rh >= 2) {
                    const auto[llh, lrh] = avl->left->heights();
                    if (llh > lrh) return single_rotate_right(avl); else return double_rotate_left_right(avl);
                } else if (rh - lh >= 2) {
                    const auto[rlh, rrh] = avl->right->heights();
                    if (rrh > rlh) return single_rotate_left(avl); else return double_rotate_right_left(avl);
                } else return avl->fix_height();
            }

            void node_fix_up(avl_node *node) {
                while (node) {
                    const auto parent = node->parent;
                    if (!try_balance(node)) return;
                    node = parent;
                }
            }

            void delete_leaf(avl_node *const node) noexcept {
                const auto parent = node->parent;
                // update min-max tags
                if (node == min) { if (node->right) min = node->right; else min = parent; }
                if (node == max) { if (node->left) max = node->left; else max = parent; }
                // remove the node from tree
                const auto child = (node->left ? node->left : node->right);
                if (parent) {
                    parent->replace(node, child);
                    node_fix_up(parent);
                } else if ((root = child)) child->parent = nullptr;
            }
        };

        avl_tree m_holes;

        uint32_t alloc_id() noexcept {
            if (const auto extract = m_holes.extract_min(); extract)
                return (extract - m_start_address) >> g_block_size_shl;
            if (m_brk == m_alloc) commit(m_alloc++);
            return m_brk++;
        }

        void release_id(const uint32_t id) noexcept {
            if (id + 1 == m_brk) {
                --m_brk;
                while (m_holes.remove_max_if(compute_base(m_brk - 1))) --m_brk;
                if ((m_alloc - m_brk) > 5) while (m_alloc-- > m_brk) release(m_alloc - 1);
            } else m_holes.add(reinterpret_cast<avl_node *>(compute_base(id)));
        }
    };
}

namespace internal {
    void* rent_block() noexcept { return block_host::instance().allocate(); }

    void return_block(void* const block) noexcept { return block_host::instance().free(block); }
}
