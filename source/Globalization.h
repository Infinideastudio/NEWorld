#pragma once
#include "stdinclude.h"
#include "../DataKit/DataKit.h"

namespace Globalization
{
    vector<std::wstring> GetLangs();
    void LoadLang(std::wstring lang);
    void Load();
    std::wstring GetStrbyKey(std::wstring key);
}