#pragma once

#if __has_include(<filesystem>)
#include <filesystem>
namespace NEWorld {
    namespace filesystem = std::filesystem;
}
#elif __has_include(<boost/filesystem.hpp>)
#include <boost/filesystem.hpp>
namespace NEWorld {
    namespace filesystem = boost::filesystem;
}
#else
#error No available filesystem library configured
#endif
