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

#include <string>
#include <vector>
#include <boost/spirit/home/support/detail/hold_any.hpp>

namespace UI
{
    namespace Data
    {
        std::vector<std::string> split(const std::string& src, std::string separate_character);
        void trim(std::string& s);
        std::string strtolower(const std::string& s);

        //Some Basic Convertions with String
        bool isDecimal(const std::string& str);
        bool isInteger(const std::string& str);
        bool isBoolean(const std::string& str);
        bool isString(const std::string& str);
        double getDecimal(const std::string& str);
        int getInteger(const std::string& str);
        bool getBoolean(const std::string& str);
        const std::string getString(const std::string& str);

        namespace Convert
        {
            boost::spirit::hold_any StrToAny(const std::string& str);
            std::string ToString(boost::spirit::hold_any var);
        }

        enum class SimpleMapFileExceptions : size_t
        {
            IOFailureOnRead, IOFailureOnWrite
        };

    }
}