/*
 * This file is part of NGWorld.
 * Then DLaboratory copied this file from NGWorld to NEWorld.
 * (C) Copyright 2016 DLaboratory
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

enum CriticalLevel
{
    CriticalLevelVerbose,
    CriticalLevelInfo,
    CriticalLevelWarning,
    CriticalLevelError,
    CriticalLevelsCount
};

static const char CriticalLevelString[][5] =
{
    "(VB)", // CriticalLevelVerbose
    "(II)", // CriticalLevelInfo
    "(WW)", // CriticalLevelWarning
    "(EE)", // CriticalLevelError
};

class LoggerForwarder
{
public:
    LoggerForwarder() {}
    virtual ~LoggerForwarder() {}
    virtual void forwardLog(const std::string &str) = 0;
};

class LoggerForwarderConsole : public LoggerForwarder
{
public:
    void forwardLog(const std::string &str);
};

class LoggerForwarderFile : public LoggerForwarder
{
private:
    const std::string m_file_name;
    std::ofstream m_fout;
public:
    LoggerForwarderFile();
    explicit LoggerForwarderFile(const std::string &file_name);
    ~LoggerForwarderFile();
    void forwardLog(const std::string &str);
};

class LoggerMessageEnding
{
};

class Logger
{
private:
    static const int forwarder_buffer_size = 8;
    std::string m_forwarder_buffer[forwarder_buffer_size];
    int m_forward_buf_position;
    std::vector<std::pair<LoggerForwarder*, bool> > m_forwarders;
    CriticalLevel m_notice_level;

    // 在调制消息头的过程中，防止频繁地分配和释放内存，
    // 提前一次性分配好一个长度为128字节的缓存空间。
    // 在不够用的情况下，再临时分配和释放所需的内存。
    char m_message_buffer[128];

public:
    Logger();
    ~Logger();

    void setNoticeLevel(CriticalLevel level)
    {
        m_notice_level = level;
    }

    void log(const std::string &str, CriticalLevel level = CriticalLevelVerbose);
};

class LoggerStream
{
private:
    const CriticalLevel m_notice_level;
    Logger *m_logger;
    std::ostringstream m_stream_buffer;

    // Initialize a LoggerStream instance without explicitly
    // declare its notive level and bound logger is prohibited.
    LoggerStream();

public:
    LoggerStream(Logger *logger, const CriticalLevel notice_level)
        : m_logger(logger), m_notice_level(notice_level) {}

    LoggerStream& operator << (const LoggerMessageEnding&)
    {
        m_logger->log(m_stream_buffer.str(), m_notice_level);
        m_stream_buffer.str("");
        return *this;
    }

    template <typename T>
    LoggerStream& operator << (const T &val)
    {
        m_stream_buffer << val;
        return *this;
    }
};

extern LoggerMessageEnding logendl;
extern Logger logger;
extern LoggerStream verbosestream;
extern LoggerStream infostream;
extern LoggerStream warningstream;
extern LoggerStream errorstream;

#endif
