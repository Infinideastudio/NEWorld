#pragma once

#include "Type.h"
#include <vector>
#include <sstream>

struct Argument {
    IType* T;
    std::string Name{};
};

struct Signature {
    int Id {};
    std::string Name{};
    std::vector<Argument> Arguments{};
};

class Parser {
public:
    explicit Parser(std::string sig): mString(std::move(sig)), mCurrent(mString.begin()) {}
    auto Get() { return Parse(); }
private:
    std::string mString;
    std::string::const_iterator mCurrent;

    [[noreturn]] void HandleExceptionReCast(const std::exception& exception) {
        std::stringstream ss {};
        ss << "Failure while parsing signature: " << mString << std::endl;
        ss << "\tWith Exception: " << exception.what() << std::endl;
        const auto offset = mCurrent - mString.begin();
        ss << "\tAt Offset: " << offset << std::endl;
        if (offset >= 10) {
            ss << "\tLocal String: " << mString.substr(offset - 10, 10) << std::endl;
        }
        else {
            ss << "\tLocal String: " << mString.substr(0, offset) << std::endl;
        }
        throw std::runtime_error(ss.str());
    }

    static int ToInt(const std::string& str) {
        int ret = 0;
        for (auto x : str) {
            if (std::isdigit(x)) ret = ret * 10 + (x -'0');
            else throw std::runtime_error(std::string("expected decimal digit but got '") + x + "'");
        }
        return ret;
    }

    std::string ReadStringAssert(const char delim, const char* message) {
        auto result = ReadString(delim);
        AssertDelim(delim, message);
        return result;
    }

    std::string ReadString(const char delim) {
        std::string result {};
        while (*mCurrent != 0 && *mCurrent != delim) result.push_back(*mCurrent++);
        return result;
    }

    void AssertDelim(const char delim, const char* message) {
        if (*mCurrent == delim) return (++mCurrent, void());
        throw std::runtime_error(message);
    }

    Signature Parse() {
        try {
            Signature i{};
            i.Id = ToInt(ReadStringAssert('$', "invalid char after id"));
            i.Name = ReadStringAssert('%', "invalid char after name");
            while (mCurrent != mString.cend()) {
                AssertDelim('%', "invalid argument start char");
                Argument argument{};
                argument.T = GetOrAdd(ReadStringAssert('$', "invalid type end char"));
                argument.Name = ReadStringAssert('%', "invalid char after type name");
                i.Arguments.push_back(std::move(argument));
            }
            return i;
        }
        catch (std::exception& e) {
            HandleExceptionReCast(e);
        }
    }
};