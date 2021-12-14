#pragma once

#include "Temp.h"
#include <string>
#include <sstream>

namespace temp {
    template<class T> using basic_string = std::basic_string<T, std::char_traits<T>, temp_alloc<T>>;
    using string = basic_string<char>;
    using u16string = basic_string<char16_t>;
    using u32string = basic_string<char32_t>;

    template<class T> using basic_istringstream = std::basic_istringstream<T, std::char_traits<T>, temp_alloc<T>>;
    template<class T> using basic_ostringstream = std::basic_ostringstream<T, std::char_traits<T>, temp_alloc<T>>;
    template<class T> using basic_stringstream = std::basic_stringstream<T, std::char_traits<T>, temp_alloc<T>>;

    using istringstream = basic_istringstream<char>;
    using u16istringstream = basic_istringstream<char16_t>;
    using u32istringstream = basic_istringstream<char32_t>;
    using ostringstream = basic_ostringstream<char>;
    using u16ostringstream = basic_ostringstream<char16_t>;
    using u32ostringstream = basic_ostringstream<char32_t>;
    using stringstream = basic_stringstream<char>;
    using u16stringstream = basic_stringstream<char16_t>;
    using u32stringstream = basic_stringstream<char32_t>;
}