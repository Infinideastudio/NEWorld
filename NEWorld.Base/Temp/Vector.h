#pragma once

#include "Temp.h"
#include <vector>

namespace temp {
    template <class T>
    using vector = std::vector<T, temp_alloc<T>>;
}