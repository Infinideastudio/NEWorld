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

#include <uilocale.h>
#include <uilogger.h>

namespace UI
{
    namespace Globalization
    {

        inline Str::Str(const std::string& str, bool globalizationNeed)
            : ds(str), gs(globalizationNeed? Service::getInstance().getStr(ds):ds) {}

        inline Str::operator std::string() const
        {
            return gs;
        }

        inline void Str::flush()
        {
            gs = Service::getInstance().getStr(ds);
        }

        void Service::attachLangFiles(std::initializer_list<std::string> filenames)
        {
            for (auto& s : filenames)
            {
                langs.insert(std::make_pair(s, Data::SimpleMapFile(basePath + s + suffix)));
            }
        }

        inline const std::string Service::getCurLang() const
        {
            return curLang;
        }

        void Service::setLang(const std::string & _lang)
        {
            auto result = langs.find(_lang);
            if (result!=langs.end())
            {
                curLang = _lang;
                curLangData = &(result->second);
            }
            else
            {
                logwarning("Language file " + _lang + "does not exist");
            }
        }

        const std::string Service::getStr(const std::string & key) const
        {
            if (!curLangData) return key;
            return curLangData->get<std::string>(key);
        }

        inline void Service::setBasePath(std::string _basePath)
        {
            basePath = _basePath;
        }

        inline std::string Service::getBasePath()
        {
            return basePath;
        }
        inline void Service::setSuffix(std::string _suffix) { suffix = _suffix; }
        inline std::string Service::getSuffix() { return suffix; }
    }
}
