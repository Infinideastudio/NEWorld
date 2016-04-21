#include "logger.h"
#include <ctime>
#include <sstream>
#include <iostream>
using namespace std;

Logger GlobalLogger;

void Logger::Log(std::string information, CriticalLevel level)
{
    stringstream ss;
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
    stringstream ss;
    for(vector<string>::iterator it = m_logs.begin(); it != m_logs.end(); ++it)
        ss << *it;
    return ss.str();
}
