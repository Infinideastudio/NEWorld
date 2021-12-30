#pragma once

#include "Temp.h"
#include <unordered_map>
#include <unordered_set>

namespace temp {
    template<class T, class Hash = std::hash<T>, class Eq = std::equal_to<T>>
    using unordered_set = std::unordered_set<T, Hash, Eq, temp_alloc<T>>;

    template<class T, class Hash = std::hash<T>, class Eq = std::equal_to<T>>
    using unordered_multiset = std::unordered_multiset<T, Hash, Eq, temp_alloc<T>>;

    template<class T, class V, class Hash = std::hash<T>, class Eq = std::equal_to<T>>
    using unordered_map = std::unordered_map<T, V, Hash, Eq, temp_alloc<std::pair<const T, V>>>;

    template<class T, class V, class Hash = std::hash<T>, class Eq = std::equal_to<T>>
    using unordered_multimap = std::unordered_multimap<T, V, Hash, Eq, temp_alloc<std::pair<const T, V>>>;
}