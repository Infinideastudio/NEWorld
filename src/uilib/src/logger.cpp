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

#include <uilogger.h>
#include <boost/date_time.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace UI
{
    namespace Logger
    {
        Logger::Logger(const std::string& path) :
            logpath(path), clogLevel(Level::trace), cerrLevel(Level::fatal), fileLevel(Level::trace), lineLevel(Level::warning)
        {
        }

        Logger::Logger(const std::string& path, Level _clogLevel, Level _cerrLevel, Level _fileLevel, Level _lineLevel) :
            logpath(path), clogLevel(_clogLevel), cerrLevel(_cerrLevel), fileLevel(_fileLevel), lineLevel(_lineLevel)
        {
        }

        void Logger::log(Level level, const std::string& message, const char * fileName, int lineNumber)
        {
            constexpr char* LevelString[] =
            {
                "trace","debug","info", "warning","error","fatal"
            };
            m_content << '[' << boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time()) << ']' << "[" << LevelString[(size_t)level] << "] ";
            if (level >= lineLevel)
                m_content << "(" << fileName << ":" << lineNumber << ") ";
            m_content << ":" << message << std::endl;
            if (m_content.str().length() > 32768 || level >= Level::error) dump();
        }

        void Logger::dump()
        {
            if (m_content.str().length() > 0)
            {
                std::ofstream file(logpath + "/" + boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time()) + ".log");
                file << m_content.str();
                m_content.clear();
                file.close();
            }
        }

        Logger service;

        void init(const std::string& path)
        {
            using namespace boost::filesystem;
            if (!exists(path)) create_directory(path);
            service = Logger(path);
        }
    }
}
