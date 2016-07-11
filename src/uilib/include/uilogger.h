/*
UILib - A Open-Source UI-Library

Copyright(C) 2016 Infinideastudio-UITeam

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and / or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#ifndef UILOGGER_H_
#define UILOGGER_H_

#include "uiplatform.h"
#include <string>
#include <sstream>

namespace UI
{
    namespace Logger
    {
        enum class Level : size_t
        {
            trace, debug, info, warning, error, fatal
        };

        class UILIB_API Logger
        {
        public:
            Logger() = default;
            Logger(const std::string& path);
            Logger(const std::string& path, Level _clogLevel, Level _cerrLevel, Level _fileLevel, Level _lineLevel);

            void log(Level level, const std::string& message, const char* fileName, int lineNumber);
            void dump();
        private:
            Level clogLevel, cerrLevel, fileLevel, lineLevel;
            std::string logpath;
            std::stringstream m_content;
        };

        UILIB_API extern Logger service;
        UILIB_API void init(const std::string& path);

#define logdebug(x)    UI::Logger::service.log(UI::Logger::Level::debug  , x, __FUNCTION__, __LINE__)
#define loginfo(x)     UI::Logger::service.log(UI::Logger::Level::info   , x, __FUNCTION__, __LINE__)
#define logwarning(x)  UI::Logger::service.log(UI::Logger::Level::warning, x, __FUNCTION__, __LINE__)
#define logerror(x)    UI::Logger::service.log(UI::Logger::Level::error  , x, __FUNCTION__, __LINE__)
#define logfatal(x)    UI::Logger::service.log(UI::Logger::Level::fatal  , x, __FUNCTION__, __LINE__)
    }
}

#endif
