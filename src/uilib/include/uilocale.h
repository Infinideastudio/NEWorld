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

#ifndef UILOCALE_H_
#define UILOCALE_H_

#include "uidata.h"
#include <unordered_map>

namespace UI
{
    namespace Globalization
    {
        class UILIB_API Str
        {
        public:
            Str(const std::string& str, bool globalizationNeed = true);
            operator std::string() const;
            void flush();

        private:
            const std::string ds; //data string
            std::string gs; //globalization string
        };

        class UILIB_API Service
        {
        public:
            Service() = default;
            void attachLangFiles(std::initializer_list<std::string> filenames);
            const std::string getCurLang() const;
            void setLang(const std::string& _lang);
            const std::string getStr(const std::string& key) const;
            static inline Service& getInstance()
            {
                static Service service;
                return service;
            }
            void setBasePath(std::string _basePath);
            std::string getBasePath();
            void setSuffix(std::string _suffix);
            std::string getSuffix();
        private:
            std::unordered_map<std::string, const Data::SimpleMapFile> langs;
            std::string curLang;
            const Data::SimpleMapFile* curLangData;
            std::string basePath = "langs/", suffix = ".lang";
        };
    }
}

#endif
