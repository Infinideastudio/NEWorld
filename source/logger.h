#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>
class Logger
{
private:
    std::vector<std::string> m_logs;

public:
    /* 根据Xorg的日志信息级别，有
     * (II) Information 信息
     * (EE) Error 错误
     * (WW) Warning 警告
     * (NI) Not Implemented 没有实现
     * 四个级别。四种级别的信息分别用括号括起来的双大写字母表示其等级。
     * 若级别未知，用(??)表示。
     *
     * 从/var/log/Xorg.0.log截取下来的样例：
    (WW) warning, (EE) error, (NI) not implemented, (??) unknown.
    (WW) The directory "/usr/share/fonts/X11/cyrillic" does not exist.
    (WW) NVIDIA(0):
    (WW) NVIDIA(0): No modes were requested; the default mode "nvidia-auto-select"
    (WW) NVIDIA(0): will be used as the requested mode.
    (WW) NVIDIA(0):
    (WW) Configured Mouse: No Device specified, looking for one...
     */
    enum CriticalLevel
    {
        CRITICAL_LEVEL_INFORMATION,
        CRITICAL_LEVEL_WARNING,
        CRITICAL_LEVEL_ERROR,
        CRITICAL_LEVEL_NOT_IMPLEMENTED
    };

    //存入一个消息，默认为(II)级别
    void Log(std::string information, CriticalLevel level = CRITICAL_LEVEL_INFORMATION);

    //导出存储的所有日志信息到一个std::string中。
    std::string ExportAll();
};


#endif
