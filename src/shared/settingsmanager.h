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
#include "type.h"

class Settings
{
public:
    //use the file name to initialize Settings and read the configuration from the file automaticly
    Settings(const std::string& filename) :m_settings(readFromFile(std::move(std::ifstream(filename)))), m_filename(filename) {}

    //save the configuration to the file which it is read from
    void save() const
    {
        writeToFile(std::move(std::ofstream(m_filename)), m_settings, m_minimal);
    }

    template <typename T>
    T get(std::string key, T defaultValue = T())
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

    const SettingsMap& getMap() const
    {
        return m_settings;
    }

private:
    SettingsMap m_settings;
    std::string m_filename;
    bool m_minimal = false;

    static SettingsMap readFromFile(std::ifstream&& file);

    static void writeToFile(std::ofstream&& file, const SettingsMap& settings, bool minimal)
    {
        for (const auto& p : settings)
            file << p.first << (minimal ? "=" : " = ") << type2string(p.second) << std::endl;
    }
};


#endif // SETTINGSMANAGER_H__
