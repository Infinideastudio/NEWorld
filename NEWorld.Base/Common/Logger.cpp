// 
// Core: Logger.cpp
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

#include <map>
#include <ctime>
#include <mutex>
#include <array>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "Logger.h"
#include "Console.h"

namespace {
    constexpr std::array<const char*, 6> levelTags = {
            "[verbose]", "[debug]", "[info]", "[warning]", "[error]", "[fatal]"
    };

    std::vector<std::ofstream> fSink{};

    Logger::Level coutLevel = Logger::Level::verbose;
    Logger::Level cerrLevel = Logger::Level::fatal;
    Logger::Level fileLevel = Logger::Level::info;
    Logger::Level lineLevel = Logger::Level::error;

    template <size_t length>
    std::string convert(int arg) {
        char arr[13];
        int siz = 0u;
        while (arg) {
            arr[siz++] = arg % 10 + '0'; // NOLINT
            arg /= 10;
        }
        std::string ret(length - siz, '0');
        ret.reserve(length);
        for (int i = siz - 1; i >= 0; i--) ret += arr[i];
        return ret;
    }

    std::string getTimeString(const char dateSplit, const char midSplit, const char timeSplit) {
        time_t timer = time(nullptr);
        tm currtime;
#if _MSC_VER
        localtime_s(&currtime, &timer); // MSVC
#else
        localtime_r(&timer, &currtime); // POSIX
#endif
        return convert<4u>(currtime.tm_year + 1900) + dateSplit + convert<2u>(currtime.tm_mon)
            + dateSplit + convert<2u>(currtime.tm_mday) + midSplit + convert<2u>(currtime.tm_hour)
            + timeSplit + convert<2u>(currtime.tm_min) + timeSplit + convert<2u>(currtime.tm_sec);
    }

    void setLogColor(const Logger::Level& level, std::stringstream& content) {
        switch (level) {
        case Logger::Level::verbose:
        case Logger::Level::debug: return (content << LColor::white, (void)0);
        case Logger::Level::info: return (content << LColor::lwhite, (void)0);
        case Logger::Level::warning: return (content << LColor::lyellow, (void)0);
        case Logger::Level::error: return (content << LColor::lred, (void)0);
        case Logger::Level::fatal: return (content << LColor::red, (void)0);
        default: return;
        }
    }

    constexpr char styleChar = '&';

    LColorFunc::ColorFunc queryColorFunc(const char style) {
        using namespace LColorFunc;
        static std::map<char, ColorFunc> map = {
                {'0', black}, {'1', red}, {'2', yellow}, {'3', green},
                {'4', cyan}, {'5', blue}, {'6', magenta}, {'7', white},
                {'8', lblack}, {'9', lred}, {'a', lyellow}, {'b', lgreen},
                {'c', lcyan}, {'d', lblue}, {'e', lmagenta}, {'f', lwhite},
        };
        const auto chNorm = (style >= 'A' && style <= 'F') ? style - 'A' + 'a' : style;
        const auto sRes = map.find(chNorm);
        return (sRes != map.end()) ? sRes->second : ColorFunc();
    }

    void chConsumeStyled(std::ostream& ostream, const char ch) {
        if (const auto cf = queryColorFunc(ch); cf)
            return (ostream << cf, (void)0);
        if (ch == styleChar) ostream << styleChar; // Escaped to `stylechar`
        else ostream << styleChar << ch; // Wrong color code
    }

    template <class Fn>
    auto transitStyleString(const std::string& str, Fn fn) {
        std::string_view vw{ str };
        std::string::size_type pos1 = 0;
        std::stringstream ss{};
        for (;;) {
            const auto pos2 = vw.find(styleChar, pos1);
            if (std::string::npos == pos2) return (ss << vw.substr(pos1, str.size()), ss.str());
            ss << vw.substr(pos1, pos2 - pos1);
            if (pos2 < str.size()) fn(ss, str[pos2 + 1]);
            pos1 = pos2 + 2;
        }
    }

    void lockedFlush(std::ostream& stream, const std::string& string) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        stream << string;
    }

    void flushConsole(const Logger::Level level, const std::string& string) {
        const auto line = transitStyleString(string, chConsumeStyled);
        if (level >= cerrLevel)
            lockedFlush(std::cerr, line);
        else if (level >= coutLevel)
            lockedFlush(std::cout, line);
    }

    void flushFiles(const Logger::Level level, const std::string& string) {
        if (level >= fileLevel) {
            const auto line = transitStyleString(string, [](auto&&, auto&&) {});
            for (auto& it : fSink) {
                lockedFlush(it, line);
                if (level >= cerrLevel) it.flush();
            }
        }
    }
}

void Logger::addFileSink(const std::string& path, const std::string& prefix) {
    std::filesystem::create_directory(path);
    fSink.emplace_back(path + prefix + "_" + getTimeString('-', '_', '-') + ".log");
}

Logger::Logger(const char* fileName, const char* funcName, int lineNumber, Level level, const char* mgr)
    :mLevel(level), mFileName(fileName), mFuncName(funcName), mLineNumber(lineNumber) {
    mContent << LColor::white << getTimeString('-', ' ', ':') << " [" << mgr << ']';
    setLogColor(level, mContent);
    mContent << levelTags[static_cast<size_t>(level)] << ' ';
}

Logger::~Logger() {
    if (mLevel >= lineLevel) {
        mContent << std::endl
            << "\tSource :\t" << mFileName << std::endl
            << "\tAt Line :\t" << mLineNumber << std::endl
            << "\tFunction :\t" << mFuncName << std::endl;
    }
    mContent << std::endl;
    const auto string = mContent.str();
    if (!fileOnly) flushConsole(mLevel, string);
    flushFiles(mLevel, string);
}