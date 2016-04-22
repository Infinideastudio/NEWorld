#include "Definitions.h"
#include "logger.h"
#include <assert.h>

Logger GlobalLogger;

void Logger::Log(std::string information, CriticalLevel level)
{
    std::stringstream ss;
    ss << '[' << clock() * 1.0 / CLOCKS_PER_SEC << "] ";
    switch(level)
    {
    case CRITICAL_LEVEL_INFORMATION:
        ss << "(II) ";
        break;
    case CRITICAL_LEVEL_WARNING:
        ss << "(WW) ";
        break;
    case CRITICAL_LEVEL_ERROR:
        ss << "(EE) ";
        break;
    case CRITICAL_LEVEL_NOT_IMPLEMENTED:
        ss << "(NI) ";
        break;
    default:
        ss << "(??) ";
    }
    ss << information << endl;
    m_logs.push_back(ss.str());
#ifdef NEWORLD_TARGET_MACOSX
    //(个人习惯)在Mac上将信息即时显示在标准输出上。
    cout << ss.str() << endl;
#endif
}

string Logger::ExportAll()
{
    std::string ret;
    for (vector<string>::iterator it = m_logs.begin(); it != m_logs.end(); ++it)
        ret += *it;
    return ret;
}

void DebugWarning(std::string msg)
{
    GlobalLogger.Log(msg, Logger::CRITICAL_LEVEL_WARNING);
}

void DebugError(std::string msg)
{
    GlobalLogger.Log(msg, Logger::CRITICAL_LEVEL_ERROR);
#ifdef _DEBUG
    assert(false);
#endif
}
