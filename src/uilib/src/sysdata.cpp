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

#include <uidata.h>
#include <fstream>
#include "uidataint.h"

namespace UI
{
    namespace Data
    {
        std::string ToString(bool arg)
        {
            return arg ? "true" : "false";
        }

        boost::spirit::hold_any UI::Data::Convert::StrToAny(const std::string& str)
        {
            if (UI::Data::isBoolean(str))
                return boost::spirit::hold_any(UI::Data::getBoolean(str));
            else
                if (UI::Data::isInteger(str))
                    return boost::spirit::hold_any(UI::Data::getInteger(str));
                else
                    if (UI::Data::isDecimal(str))
                        return boost::spirit::hold_any(UI::Data::getDecimal(str));
                    else
                        if (UI::Data::isString(str))
                            return boost::spirit::hold_any(UI::Data::getString(str));
                        else
                            logwarning("Failed to read value:" + str + ", which maybe not supported.");
            return boost::spirit::hold_any();
        }

        std::vector<std::string> split(const std::string& src, std::string separate_character)
        {
            std::vector<std::string> strs;
            int separate_characterLen = separate_character.size();
            int last_position = 0, index = -1;
            while (-1 != (index = src.find(separate_character, last_position)))
            {
                strs.push_back(src.substr(last_position, index - last_position));
                last_position = index + separate_characterLen;
            }
            std::string lastString = src.substr(last_position);
            if (!lastString.empty())
                strs.push_back(lastString);
            return strs;
        };

        void trim(std::string& s)
        {
            if (s.empty())
                return;

            s.erase(0, s.find_first_not_of(" "));
            s.erase(s.find_last_not_of(" ") + 1);
        }

        std::string strtolower(const std::string& s)
        {
            std::string str = s;
            std::transform(str.begin(), str.end(), str.begin(), tolower);
            return str;
        }

        //Some Basic Convertions with String
        bool isDecimal(const std::string& str)
        {
            if (str.empty()) return false;
            bool dot = false;
            for (char c : str)
            {
                if (c == '.' && !dot) dot = true;
                else if (c<'0' || c>'9') return false;
            }
            return true;
        }

        bool isInteger(const std::string& str)
        {
            if (str.empty()) return false;
            for (char c : str)
                if (c<'0' || c>'9') return false;
            return true;
        }

        bool isBoolean(const std::string& str)
        {
            if (str.empty()) return false;
            return strtolower(str) == "true" || strtolower(str) == "false";
        }

        bool isString(const std::string & str)
        {
            char c1 = str[0], c2 = str[str.length() - 1];
            return (c1 == '"') && (c2 == '"');
        }

        double getDecimal(const std::string& str)
        {
            return std::stod(str);
        }

        int getInteger(const std::string& str)
        {
            return std::stoi(str);
        }

        bool getBoolean(const std::string& str)
        {
            return strtolower(str) == "true";
        }

        const std::string getString(const std::string & str)
        {
            return str.substr(1, str.length() - 2);
        }

        std::string Convert::ToString(boost::spirit::hold_any var)
        {
            if (var.type() == typeid(bool))
            {
                return var.cast<bool>() ? "true" : "false";
            }
            else if (var.type() == typeid(int))
            {
                return std::to_string(var.cast<int>());
            }
            else if (var.type() == typeid(double))
            {
                return std::to_string(var.cast<double>());
            }
            else if (var.type() == typeid(std::string))
            {
                return '"' + var.cast<std::string>() + '"';
            }
            else 
                logwarning("Failed to handle type " + std::string(var.type().name()) + " which maybe not supported.");
            return "";
        }

        SimpleMapFile::SimpleMapFile(const std::string& filename)
            :m_settings(), m_filename(filename)
        {
            std::ifstream file(m_filename);
            try
            {
                if (file.good())
                {
                    while (!file.eof())
                    {
                        std::string line;
                        std::getline(file, line);
                        auto vk = split(line, "=");
                        if (vk.size() != 2) continue;
                        std::string key = vk[0], value = vk[1];
                        trim(key);
                        trim(value);
                        m_settings[key] = Convert::StrToAny(value);
                    }
                }
                else
                    throw SimpleMapFileExceptions::IOFailureOnRead;
            }
            catch (SimpleMapFileExceptions)
            {
                logerror("Failed To Load File \"" + filename + "\",May Be Caused By Illegal Path");
            }
            file.close();
        }

        //将配置文件保存到读取的文件
        void SimpleMapFile::save()
        {
            std::ofstream file(m_filename);
            try
            {
                if (file.good())
                {
                    for (const auto& p : m_settings)
                    {
                        file << p.first << " = " << Convert::ToString(p.second) << std::endl;
                    }
                }
                else
                    throw SimpleMapFileExceptions::IOFailureOnWrite;
            }
            catch (SimpleMapFileExceptions)
            {
                logerror("Failed To Load File \"" + m_filename + "\".Is The Target A Read-Only File?");
            }
            file.close();
        }

        //设置某一项配置
        void SimpleMapFile::set(const std::string& key, const boost::spirit::hold_any value)
        {
            m_settings[key] = value;
        }

        bool SimpleMapFile::has(const std::string& key) const
        {
            return m_settings.find(key) != m_settings.cend();
        }
    }
}
