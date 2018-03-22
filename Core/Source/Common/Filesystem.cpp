// 
// Core: Filesystem.cpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#include <string>
#include <cstring>
#include "Filesystem.h"
#include "StringUtils.h"

#if (BOOST_OS_CYGWIN || BOOST_OS_WINDOWS)
#include "Internals/Windows.hpp"
#endif

namespace {
    constexpr const char* pathSep =
#if (BOOST_OS_WINDOWS)
        ";";
#else
        ":";
#endif

    constexpr const char* sep =
#if (BOOST_OS_WINDOWS)
        "\\";
#else
        "/";
#endif

    std::string getEnv(const std::string& varName) {
        if (auto value = std::getenv(varName.c_str()); value)
            return value;
        return "";
    }

    auto getDirectoryListFromDelimitedString(const std::string& str) {
        std::vector<std::string> dirs{};
        if (!str.empty())
            dirs = split(str, pathSep[0]);
        return dirs;
    }

    filesystem::path searchPath(const std::string& file) {
        if (file.empty()) return "";
        // Drat! I have to do it the hard way.
        for (auto&& pth : getDirectoryListFromDelimitedString(getEnv("PATH")))
            if (auto p = filesystem::path(pth) / file; filesystem::exists(p) && filesystem::is_regular_file(p))
                return p.make_preferred();
        return "";
    }

    filesystem::path makeWithString(const char* argv0) {
        std::error_code ec;
        auto p(filesystem::canonical(argv0, filesystem::current_path(), ec));
        return ec ? p.make_preferred() : "";
    }

    auto makeWithString(const std::string& str) { return makeWithString(str.c_str()); }

    filesystem::path executablePathFallback(const char* argv0) {
        if (argv0 == nullptr) return "";
        if (argv0[0] == 0) return "";
        // Check if the path is full path
        if (strstr(argv0, sep) != nullptr)
            if (auto pth = makeWithString(argv0); !pth.empty()) return pth;
        // Not full path, try to search in PATH
        if (auto pth = searchPath(argv0); !pth.empty()) return pth;
        // Failed. Return anyway
        return makeWithString(argv0);
    }

    class ExecPathHelper {
    public:
        static auto& getInstance() { static ExecPathHelper ins; return ins; }
        operator filesystem::path() {
            if(mPath.empty())
                init(nullptr);
            return mPath;
        }
        void init(const char* argv0) { mPath = get(argv0).remove_filename(); }
    private:
        filesystem::path get(const char* argv0) {
            auto ret = executablePathWorker();
            if (ret.empty())
                ret = executablePathFallback(argv0);
            return filesystem::absolute(ret.make_preferred());
        }
        filesystem::path mPath;
        static filesystem::path executablePathWorker();
    };
}

#if (BOOST_OS_CYGWIN || BOOST_OS_WINDOWS)
#include <string>
#include <vector>

filesystem::path ExecPathHelper::executablePathWorker() {
    std::vector<wchar_t> buf(32768, 0);
    return (GetModuleFileNameW(nullptr, buf.data(), buf.size()) ? buf.data() : L"");
}

#elif (BOOST_OS_SOLARIS)

#include <cstdlib>
#include <string>

filesystem::path ExecPathHelper::executablePathWorker() {
    if (auto pathString = getexecname(); !pathString.empty())
        return makeWithString(pathString);
    return "";
}

#elif (BOOST_OS_QNX)

#include <fstream>
#include <string>

filesystem::path ExecPathHelper::executablePathWorker() {
    filesystem::path ret;
    std::string s;
    std::ifstream ifs("/proc/self/exefile");
    std::getline(ifs, s);
    if (ifs.fail() || s.empty()) {
        return ret;
    }
    return ret;
}

#elif (BOOST_OS_MACOS || BOOST_OS_IOS)

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

#include <mach-o/dyld.h>

filesystem::path ExecPathHelper::executablePathWorker() {
    filesystem::path ret;
    std::vector<char> buf(1024, 0);
    uint32_t size = static_cast<uint32_t>(buf.size());
    bool havePath = false;
    bool shouldContinue = true;
    do {
        if (_NSGetExecutablePath(buf.data(), &size) == -1) {
            buf.resize(size << 1);
            std::fill(std::begin(buf), std::end(buf), 0);
        }
        else {
            shouldContinue = false;
            if (buf.at(0) != 0) {
                havePath = true;
            }
        }
    } while (shouldContinue);
    if (!havePath) {
        return ret;
    }
    return makeWithString(std::string(buf.data(), size));
}

#elif (BOOST_OS_ANDROID || BOOST_OS_HPUX || BOOST_OS_LINUX || BOOST_OS_UNIX)

filesystem::path ExecPathHelper::executablePathWorker() {
    filesystem::path ret;
    std::error_code ec;
    auto linkPath = filesystem::read_symlink("/proc/self/exe", ec);
    if (ec)
        return ret;
    return makeWithString(linkPath.string());
}

#elif (BOOST_OS_BSD)

#include <string>
#include <vector>

#if (BOOST_OS_BSD_FREE)

#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdlib.h>

filesystem::path ExecPathHelper::executablePathWorker() {
    filesystem::path ret;
    int mib[4]{ CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
    size_t size;
    if (sysctl(mib, 4, nullptr, &size, nullptr, 0) == -1) return ret;
    std::vector<char> buf(size + 1, 0);
    if (sysctl(mib, 4, buf.data(), &size, nullptr, 0) == -1) return ret;
    return makeWithString(std::string(buf.data(), size));
}

#elif (BOOST_OS_BSD_NET)

filesystem::path ExecPathHelper::executablePathWorker() {
    filesystem::path ret;
    std::error_code ec;
    auto linkPath = filesystem::read_symlink("/proc/curproc/exe", ec);
    if (ec) 
        return ret;
    return makeWithString(linkPath.string());
}

#elif BOOST_OS_BSD_DRAGONFLY

#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdlib.h>

filesystem::path ExecPathHelper::executablePathWorker() {
    filesystem::path ret;
    std::error_code ec;
    auto linkPath = filesystem::read_symlink("/proc/curproc/file", ec);
    if (ec) {
        int mib[4]{ CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
        size_t size;
        if (sysctl(mib, 4, nullptr, &size, nullptr, 0) != -1) {
            std::vector<char> buf(size + 1, 0);
            if (sysctl(mib, 4, buf.data(), &size, nullptr, 0) != -1) {
                linkPath = std::string(buf.data(), size);
            }
        }
    }
    return makeWithString(linkPath.string());
}

#endif

#else

filesystem::path ExecPathHelper::executablePathWorker() { return ""; }

#endif

filesystem::path executablePath() { return ExecPathHelper::getInstance(); }

void fsInit(const char *argv0) { ExecPathHelper::getInstance().init(argv0); }

filesystem::path assetDir(const char *moduleName) { return executablePath() / "Assets" / moduleName; }

filesystem::path assetDir(const std::string &moduleName) { return assetDir(moduleName.c_str()); }

filesystem::path dataDir(const char* moduleName) { return executablePath() / "Data" / moduleName; }

filesystem::path dataDir(const std::string& moduleName) { return dataDir(moduleName.c_str()); }
