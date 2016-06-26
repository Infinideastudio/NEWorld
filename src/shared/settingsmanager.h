/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
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

#ifndef SETTINGSMANAGER_H__
#define SETTINGSMANAGER_H__

#include <map>
#include <string>
#include <fstream>
#include <boost/spirit/home/support/detail/hold_any.hpp>
#include "utils.h"

class Settings
{
public:
    //使用文件名初始化Settings，自动从该文件读取配置
    Settings(const std::string& filename) :m_settings(readFromFile(std::move(std::ifstream(filename)))), m_filename(filename) {}

    //将配置文件保存到读取的文件
    void save()
    {
        writeToFile(std::move(std::ofstream(m_filename)), m_settings, m_minimal);
    }

    //从配置文件中获取配置
    template<class T>
    T get(std::string key, T defaultValue)
    {
        strtolower(key);
        auto result = m_settings.find(key);
        if (result != m_settings.end())
            return result->second.cast<T>();
        else
        {
            m_settings[key] = defaultValue;
            return defaultValue;
        }
    }

    boost::spirit::hold_any get(std::string key)
    {
        strtolower(key);
        auto result = m_settings.find(key);
        if (result != m_settings.end()) return result->second;
        return boost::spirit::hold_any();
    }

    //设置某一项配置
    void set(std::string key, const boost::spirit::hold_any value)
    {
        m_settings[key] = value;
    }

    bool have(std::string key)
    {
        return m_settings.find(key) != m_settings.end();
    }

    void setMinimal(bool minimal)
    {
        m_minimal = minimal;
    }

    using SettingsMap = std::map<std::string, boost::spirit::hold_any>;

    const SettingsMap& getMap()
    {
        return m_settings;
    }

private:
    SettingsMap m_settings;
    std::string m_filename;
    bool m_minimal = false;
    //从文件读取配置
    static SettingsMap readFromFile(std::ifstream&& file);

    //把配置写入到文件
    static void writeToFile(std::ofstream&& file, const SettingsMap& settings, bool minimal);
};


#endif // SETTINGSMANAGER_H__
