#include <uilib.h>
#include <iomanip>
#include <ctime>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
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
            constexpr const char* LevelString[] =
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
            std::ofstream file(logpath + "/" + boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time()) + ".log");
            file << m_content.str();
            m_content.clear();
            file.close();
        }

        Logger service;

        UILIB_API void init(const char* path)
        {
            using namespace boost::filesystem;
            if (!exists(path))
                create_directory(path);
            service = Logger(path);
        }
    }
}
