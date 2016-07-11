#include <uilib.h>

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
namespace UI
{
    namespace Data
    {
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

    }
}