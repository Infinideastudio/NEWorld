// 
// Core: Exception.h
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

#pragma once

#include <string>
#include <chrono>

#pragma warning(disable: 4673)

namespace StringUtils {
    template <typename... Args>
    std::string FormatString(const char* lpStr, Args&&... args);
}

namespace Exception {
    namespace detail_ {
        struct ExceptionStorage {
            ExceptionStorage(std::exception_ptr nestedException, std::chrono::system_clock::time_point const& time,
                             std::string file, unsigned line, std::string src, std::string desc) noexcept
                : mNestedException(nestedException), mTime(time), mFile(file), mLine(line), mSource(src),
                  mDescription(desc) { }

            std::exception_ptr mNestedException;
            std::chrono::system_clock::time_point mTime;
            std::string mFile;
            unsigned mLine;
            std::string mSource;
            std::string mDescription;
        };
    }

    class Exception
        : protected detail_::ExceptionStorage, public virtual std::exception {
    public:
        template <typename... Args>
        Exception(std::exception_ptr nestedException, const char* Src, const char* File, unsigned Line,
                  const char* Desc, Args&&... args) noexcept
            : detail_::ExceptionStorage {
                nestedException, std::chrono::system_clock::now(), File, Line, Src,
                StringUtils::FormatString(Desc, std::forward<Args>(args)...)
            } { }

        template <typename... Args>
        Exception(const char* Src, const char* File, unsigned Line, const char* Desc, Args&&... args) noexcept
            : detail_::ExceptionStorage {
                {}, std::chrono::system_clock::now(), File, Line, Src,
                StringUtils::FormatString(Desc, std::forward<Args>(args)...)
            } { }

        ~Exception() = default;

        std::chrono::system_clock::time_point GetTime() const noexcept { return mTime; }

        const char* GetFile() const noexcept { return mFile.c_str(); }

        unsigned GetLine() const noexcept { return mLine; }

        const char* GetSource() const noexcept { return mSource.c_str(); }

        const char* GetDesc() const noexcept { return mDescription.c_str(); }

        std::exception_ptr GetNestedException() const noexcept { return mNestedException; }

        const char* what() const noexcept override { return mDescription.c_str(); }
    };

#define DeclareException(ExceptionClass, ExtendException, DefaultDescription) \
    class ExceptionClass : public ExtendException\
    {\
    public:\
        typedef ExtendException BaseException;\
        ExceptionClass(const char* Src, const char* File, unsigned Line) noexcept\
            : BaseException(Src, File, Line, DefaultDescription)\
        {\
        }\
        ExceptionClass(std::exception_ptr nestedException, const char* Src, const char* File, unsigned Line) noexcept\
            : BaseException(nestedException, Src, File, Line, DefaultDescription)\
        {\
        }\
        template <typename... Args>\
        ExceptionClass(const char* Src, const char* File, unsigned Line, const char* Desc, Args&&... args) noexcept\
            : BaseException(Src, File, Line, Desc, std::forward<Args>(args)...)\
        {\
        }\
        template <typename... Args>\
        ExceptionClass(std::exception_ptr nestedException, const char* Src, const char* File, unsigned Line, const char* Desc, Args&&... args) noexcept\
            : BaseException(nestedException, Src, File, Line, Desc, std::forward<Args>(args)...)\
        {\
        }\
    }
}

#define nw_throw(ExceptionClass, ...)\
    do { throw ExceptionClass{ __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ }; } while (false)
#define nw_throwwithnested(ExceptionClass, ...)\
    do { throw ExceptionClass{ std::current_exception(), __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ }; } while (false)
