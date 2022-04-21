#pragma once

#include <mutex>
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <kls/Object.h>

class MessageBus {
    class SlotBase : public kls::PmrBase {
    protected:
        std::vector<std::shared_ptr<PmrBase>> PrepareInvokeList();
        std::mutex mLock;
        std::vector<std::weak_ptr<PmrBase>> mListeners;
    };

public:
    template<class Tm>
    class Slot final : public SlotBase {
        class ListenerBase : public kls::PmrBase {
            using Invoke = void (*)(ListenerBase *, void *, const Tm &) noexcept;
        public:
            explicit ListenerBase(const Invoke fn) noexcept
                    : Function(fn) {}

            Invoke Function;
        };

        template<class Fn>
        class Listener final : public ListenerBase {
        public:
            explicit Listener(Fn package)
                    : ListenerBase(Invoke), mPackage(std::move(package)) {}

        private:
            static void Invoke(ListenerBase *ths, void *s, const Tm &arg) noexcept {
                static_cast<Listener *>(ths)->mPackage(s, arg);
            }

            Fn mPackage;
        };

    public:
        void Send(void *sender, const Tm &arg) {
            const auto invokes = PrepareInvokeList();
            for (const auto &x: invokes) {
                const auto x2 = static_cast<ListenerBase *>(x.get());
                if (x2->Function) { std::invoke(x2->Function, x2, sender, arg); }
            }
        }

        template<class Fn>
        std::shared_ptr<PmrBase> Listen(Fn fn) {
            const auto s = std::make_shared<Listener<Fn>>(std::move(fn));
            {
                std::lock_guard<std::mutex> lk{mLock};
                mListeners.push_back(s);
            }
            return s;
        }
    };

    template<class Tm>
    Slot<Tm> *Get(const std::string &name) noexcept {
        const auto search = mSlots.find(name);
        if (search != mSlots.end()) { return dynamic_cast<Slot<Tm> *>(search->second.get()); }
        auto ptr = std::make_unique<Slot<Tm>>();
        const auto ret = ptr.get();
        mSlots.insert_or_assign(name, std::move(ptr));
        return ret;
    }

    static MessageBus &Default();
private:
    std::unordered_map<std::string, std::unique_ptr<kls::PmrBase>> mSlots;
};

using NullArg = int;
