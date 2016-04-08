#include "Globalization.h"
#include "../DataKit/DataKit.h"

namespace Globalization
{
    DataKit::cl_file ls;
    DataKit::cl_file* l = nullptr;

std::wstring Cur_Lang = L"zh_CN";

vector<std::wstring> GetLangs()
{
    return vector<std::wstring>();
}

void LoadLang(std::wstring lang)
{
    if (l != nullptr) {
        delete l;
    }
    l = new DataKit::cl_file;
    l->open(L"locale/" + lang + L".dtsf");
}

void Load()
{
    LoadLang(Cur_Lang);
}

std::wstring GetStrbyKey(std::wstring key)
{
    DataKit::cl_str* res = (DataKit::cl_str*)(*l)[key];
    if (res)
        return *res->val;
    else
        return key;
}
}