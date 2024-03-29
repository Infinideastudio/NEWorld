#pragma once

#include <string>

namespace Globalization {
    struct Line {
        std::string str;
        int id;
    };

    extern std::string Cur_Lang;

    bool LoadLang(const std::string& lang);

    bool Load();

    std::string GetStrbyid(int id);

    std::string GetStrbyKey(std::string key);
}