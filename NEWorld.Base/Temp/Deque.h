#pragma onceonce

#include "Temp.h"
#include <deque>

namespace temp {
    template <class T>
    using deque = std::deque<T, temp_alloc<T>>;
}