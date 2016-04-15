#pragma once
#include "stdinclude.h"

namespace Globalization
{
    void Load();
    void LoadLang(string lang);
    void Finalize();
    string GetStr(string key);

    struct LangInfo
    {
        string symbol, eng_symbol, name;
    };

    vector<LangInfo> GetLangs();
    string& Cur_Lang();
}