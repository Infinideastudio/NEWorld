#include "Globalization.h"

namespace Globalization
{

int count;
string Cur_Lang = "zh_CN", Cur_Symbol = "", Cur_Name = "";
map<int, Line> Lines;
map<string, int> keys;

bool LoadLang(string lang)
{
    std::ifstream f("Lang/" + lang + ".lang");
    if (f.bad())
    {
        exit(-101);
        return false;
    }
    Lines.clear();
    Cur_Lang = lang;
    f >> Cur_Symbol;
    f.get();
    getline(f, Cur_Name);
    for (int i = 0; i < count; i++)
    {
        getline(f, Lines[i].str);
    }
    f.close();
    return true;
}

bool Load()
{
    std::ifstream f("Lang/Keys.lk");
    if (f.bad()) return false;
    f >> count;
    f.get();
    for (int i = 0; i < count; i++)
    {
        string temp;
        getline(f, temp);
        keys[temp] = i;
    }
    f.close();
    return LoadLang(Cur_Lang);
}

string GetStrbyid(int id)
{
    return Lines[id].str;
}

string GetStrbyKey(string key)
{
    return Lines[keys[key]].str;
}
}