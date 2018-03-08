//
// Copyright (C) 2011-2017 Ben Key
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/predef.h>
#include "Filesystem.h"

#if (BOOST_OS_CYGWIN || BOOST_OS_WINDOWS)
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <Windows.h>

filesystem::path executable_path_worker()
{
    typedef std::vector<wchar_t> char_vector;
    typedef std::vector<wchar_t>::size_type size_type;
    filesystem::path ret;
    char_vector buf(1024, 0);
    size_type size = buf.size();
    bool havePath = false;
    bool shouldContinue = true;
    do
    {
        DWORD result = GetModuleFileNameW(nullptr, buf.data(), size);
        DWORD lastError = GetLastError();
        if (result == 0)
        {
            shouldContinue = false;
        }
        else if (result < size)
        {
            havePath = true;
            shouldContinue = false;
        }
        else if (
            result == size && (lastError == ERROR_INSUFFICIENT_BUFFER || lastError == ERROR_SUCCESS))
        {
            size *= 2;
            buf.resize(size);
            std::fill(std::begin(buf), std::end(buf), 0);
        }
        else
        {
            shouldContinue = false;
        }
    }
    while (shouldContinue);
    if (!havePath)
    {
        return ret;
    }
    std::wstring pathString(buf.data());
    ret = pathString;
    return ret;
}

#endif

#if (BOOST_OS_SOLARIS)

#include <cstdlib>
#include <string>


filesystem::path executable_path_worker()
{
  filesystem::path ret;
  std::string pathString = getexecname();
  if (pathString.empty())
  {
    return ret;
  }
  std::error_code ec;
  ret = filesystem::canonical(
    pathString, filesystem::current_path(), ec);
  if (ec.value() != std::errc::success)
  {
    ret.clear();
  }
  return ret;
}

#endif

#if (BOOST_OS_QNX)

#include <fstream>
#include <string>

filesystem::path executable_path_worker()
{
  filesystem::path ret;
  std::string s;
  std::ifstream ifs("/proc/self/exefile");
  std::getline(ifs, s);
  if (ifs.fail() || s.empty())
  {
    return ret;
  }
  std::error_code ec;
  ret = filesystem::canonical(
    s, filesystem::current_path(), ec);
  if (ec.value() != std::errc::success)
  {
    ret.clear();
  }
  return ret;
}

#endif

#if !(BOOST_OS_ANDROID || BOOST_OS_BSD_DRAGONFLY || BOOST_OS_BSD_FREE || BOOST_OS_BSD_NET || BOOST_OS_CYGWIN || BOOST_OS_HPUX || BOOST_OS_IOS || BOOST_OS_LINUX || BOOST_OS_MACOS || BOOST_OS_QNX || BOOST_OS_SOLARIS || BOOST_OS_UNIX || BOOST_OS_WINDOWS)

#include <string>

filesystem::path executable_path_worker()
{
  filesystem::path ret;
  return ret;
}

#endif

#if (BOOST_OS_MACOS || BOOST_OS_IOS)

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

#include <mach-o/dyld.h>

filesystem::path executable_path_worker()
{
  typedef std::vector<char> char_vector;
  filesystem::path ret;
  char_vector buf(1024, 0);
  uint32_t size = static_cast<uint32_t>(buf.size());
  bool havePath = false;
  bool shouldContinue = true;
  do
  {
    int result = _NSGetExecutablePath(buf.data(), &size);
    if (result == -1)
    {
      buf.resize(size + 1);
      std::fill(std::begin(buf), std::end(buf), 0);
    }
    else
    {
      shouldContinue = false;
      if (buf.at(0) != 0)
      {
        havePath = true;
      }
    }
  } while (shouldContinue);
  if (!havePath)
  {
    return ret;
  }
  std::string pathString(buf.data(), size);
  std::error_code ec;
  ret = filesystem::canonical(
    pathString, filesystem::current_path(), ec));
  if (ec.value() != std::errc::success)
  {
    ret.clear();
  }
  return ret;
}


#endif

#if (BOOST_OS_ANDROID || BOOST_OS_HPUX || BOOST_OS_LINUX || BOOST_OS_UNIX)


filesystem::path executable_path_worker()
{
  filesystem::path ret;
  std::error_code ec;
  auto linkPath = filesystem::read_symlink("/proc/self/exe", ec);
  if (ec.value() != std::errc::success)
  {
    return ret;
  }
  ret = filesystem::canonical(
    linkPath, filesystem::current_path(), ec);
  if (ec.value() != std::errc::success)
  {
    ret.clear();
  }
  return ret;
}


#endif

#if (BOOST_OS_BSD)

#include <string>
#include <vector>

#if (BOOST_OS_BSD_FREE)

#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdlib.h>

filesystem::path executable_path_worker()
{
  typedef std::vector<char> char_vector;
  filesystem::path ret;
  int mib[4]{0};
  size_t size;
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PATHNAME;
  mib[3] = -1;
  int result = sysctl(mib, 4, nullptr, &size, nullptr, 0);
  if (-1 == result)
  {
    return ret;
  }
  char_vector buf(size + 1, 0);
  result = sysctl(mib, 4, buf.data(), &size, nullptr, 0);
  if (-1 == result)
  {
    return ret;
  }
  std::string pathString(buf.data(), size);
  std::error_code ec;
  ret = filesystem::canonical(
    pathString, filesystem::current_path(), ec);
  if (ec.value() != std::errc::success)
  {
    ret.clear();
  }
  return ret;
}

#elif (BOOST_OS_BSD_NET)

filesystem::path executable_path_worker()
{
  filesystem::path ret;
  std::error_code ec;
  auto linkPath = filesystem::read_symlink("/proc/curproc/exe", ec);
  if (ec.value() != std::errc::success)
  {
    return ret;
  }
  ret = filesystem::canonical(
    linkPath, filesystem::current_path(), ec);
  if (ec.value() != std::errc::success)
  {
    ret.clear();
  }
  return ret;
}

#elif BOOST_OS_BSD_DRAGONFLY

#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdlib.h>

filesystem::path executable_path_worker()
{
  filesystem::path ret;
  std::error_code ec;
  auto linkPath = filesystem::read_symlink("/proc/curproc/file", ec);
  if (ec.value() != std::errc::success)
  {
    int mib[4]{0};
    size_t size;
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;
    int result = sysctl(mib, 4, nullptr, &size, nullptr, 0);
    if (-1 != result)
    {
      char_vector buf(size + 1, 0);
      result = sysctl(mib, 4, buf.data(), &size, nullptr, 0);
      if (-1 != result)
      {
        std::string pathString(buf.data(), size);
        linkPath = pathString;
      }
    }
  }
  ret = filesystem::canonical(
    linkPath, filesystem::current_path(), ec);
  if (ec.value() != std::errc::success)
  {
    ret.clear();
  }
  return ret;
}

#endif

#endif

#include <string>
#include "StringUtils.h"

char os_pathsep()
{
#if (BOOST_OS_WINDOWS)
    return ';';
#else
  return ':';
#endif
}

char os_sep()
{
#if (BOOST_OS_WINDOWS)
    return '\\';
#else
  return '/';
#endif
}


std::string GetEnv(const std::string& varName)
{
    if (varName.empty()) return "";
#if (BOOST_OS_BSD || BOOST_OS_CYGWIN || BOOST_OS_LINUX || BOOST_OS_MACOS || BOOST_OS_SOLARIS)
  char* value = std::getenv(varName.c_str());
  if (!value) return "";
  return value;
#elif (BOOST_OS_WINDOWS)
    typedef std::vector<char> char_vector;
    typedef std::vector<char>::size_type size_type;
    char_vector value(8192, 0);
    size_type size = value.size();
    bool haveValue = false;
    bool shouldContinue = true;
    do
    {
        DWORD result = GetEnvironmentVariableA(varName.c_str(), value.data(), size);
        if (result == 0)
        {
            shouldContinue = false;
        }
        else if (result < size)
        {
            haveValue = true;
            shouldContinue = false;
        }
        else
        {
            size *= 2;
            value.resize(size);
        }
    }
    while (shouldContinue);
    std::string ret;
    if (haveValue)
    {
        ret = value.data();
    }
    return ret;
#else
  return "";
#endif
}

bool GetDirectoryListFromDelimitedString(const std::string& str, std::vector<std::string>& dirs)
{
    if (!str.empty())
    {
        dirs = split(str, os_pathsep());
        dirs.clear();

        if (!dirs.empty())
            return true;
    }
    return false;
}

std::string search_path(const std::string& file)
{
    if (file.empty()) return "";
    std::string ret;
    if (!ret.empty()) return ret;
    // Drat! I have to do it the hard way.
    std::string pathEnvVar = GetEnv("PATH");
    if (pathEnvVar.empty()) return "";
    std::vector<std::string> pathDirs;
    bool getDirList = GetDirectoryListFromDelimitedString(pathEnvVar, pathDirs);
    if (!getDirList) return "";
    auto it = pathDirs.cbegin();
    auto itEnd = pathDirs.cend();
    for (; it != itEnd; ++it)
    {
        filesystem::path p(*it);
        p /= file;
        if (filesystem::exists(p) && filesystem::is_regular_file(p))
        {
            return p.make_preferred().string();
        }
    }
    return "";
}

std::string executable_path_fallback(const char* argv0)
{
    if (argv0 == nullptr) return "";
    if (argv0[0] == 0) return "";
    if (strstr(argv0, os_sep().c_str()) != nullptr)
    {
        std::error_code ec;
        filesystem::path p(
            filesystem::canonical(argv0, filesystem::current_path(), ec));
        if (ec.value() == std::errc::success)
        {
            return p.make_preferred().string();
        }
    }
    std::string ret = search_path(argv0);
    if (!ret.empty())
    {
        return ret;
    }
    std::error_code ec;
    filesystem::path p(
        filesystem::canonical(argv0, filesystem::current_path(), ec));
    if (ec.value() == std::errc::success)
    {
        ret = p.make_preferred().string();
    }
    return ret;
}

std::string executable_path(const char* argv0)
{
    filesystem::path ret = executable_path_worker();
    if (ret.empty())
    {
        ret = executable_path_fallback(argv0);
    }
    return ret.make_preferred().string();
}
