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

#ifndef UIDATA_H_
#define UIDATA_H_

#include "uilogger.h"
#include <map>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>

namespace UI
{
    namespace Data
    {
        class UILIB_API SimpleMapFile
        {
        public:
            SimpleMapFile(const std::string& filename);
            void save();
            bool has(const std::string& key) const;
            void set(const std::string& key, const boost::spirit::hold_any value);
            template<class T>
            T get(const std::string& key, T defaultValue = T()) const
            {
                try
                {
                    auto result = m_settings.find(key);
                    return (result != m_settings.end()) ? result->second.cast<T>() : defaultValue;
                }
                catch (boost::spirit::bad_any_cast e)
                {
                    logerror("boost bad cast:" + std::string(e.from) + " to " + std::string(e.to));
                    UI::Logger::service.dump();
                }

                return T();
            }
        private:
            std::map<std::string, boost::spirit::hold_any> m_settings;
            std::string m_filename;
        };
    }
}

#endif
