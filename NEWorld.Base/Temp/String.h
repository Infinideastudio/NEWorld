#pragma once

#include "Temp.h"
#include <string>

namespace temp {
    template<class T> using basic_string = std::basic_string<T, std::char_traits<T>, temp_alloc<T>>;
    using string = temp_basic_string<char>;
    using u8string = temp_basic_string<char8_t>;
    using u16string = temp_basic_string<char16_t>;
}