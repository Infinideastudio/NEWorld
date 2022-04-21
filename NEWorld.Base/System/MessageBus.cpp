#include "MessageBus.h"

std::vector<std::shared_ptr<kls::PmrBase>> MessageBus::SlotBase::PrepareInvokeList() {
    std::vector<std::shared_ptr<kls::PmrBase>> invokes{};
    std::lock_guard<std::mutex> lk{mLock};
    invokes.reserve(mListeners.size());
    for (auto& x : mListeners) {
        if (auto ptr = x.lock(); ptr) { invokes.push_back(std::move(ptr)); }
    }
    if (mListeners.size() > 4 * invokes.size()) {
        mListeners.clear();
        if (mListeners.capacity() > 8 * invokes.size()) { mListeners.shrink_to_fit(); }
        for (auto& x : invokes) { mListeners.push_back(x); }
    }
    return invokes;
}

MessageBus& MessageBus::Default() {
    static MessageBus defaultBus{};
    return defaultBus;
}
