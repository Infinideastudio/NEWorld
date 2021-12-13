#pragma once

#include "Temp.h"
#include <map>
#include <set>

namespace temp {
    template<class T, class Pr = std::less<T>>
    using set = std::set<T, Pr, temp_alloc<T>>;

    template<class T, class Pr = std::less<T>>
    using multiset = std::multiset<T, Pr, temp_alloc<T>>;

    template<class T, class V, class Pr = std::less<T>>
    using map = std::map<T, V, Pr, temp_alloc<std::pair<const T, V>>>;

    template<class T, class V, class Pr = std::less<T>>
    using multimap = std::multimap<T, V, Pr, temp_alloc<std::pair<const T, V>>>;
}