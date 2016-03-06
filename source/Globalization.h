#pragma once
#include "stdinclude.h"

namespace Globalization
{
struct Line
{
    string str;
    int id;
};

extern int count;
extern map<int, Line> Lines;
extern map<string, int> keys;
extern string Cur_Lang;

bool LoadLang(string lang);
bool Load();
string GetStrbyid(int id);
string GetStrbyKey(string key);
}