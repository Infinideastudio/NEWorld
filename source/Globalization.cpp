/*
 * NEWorld: An free game with similar rules to Minecraft.
 * Copyright (C) 2016 NEWorld Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Globalization.h"

namespace Globalization
{

int count;
string Cur_Lang = "zh_CN", Cur_Symbol = "", Cur_Name = "";
map<int, Line> Lines;
map<string, int> keys;

bool LoadLang(string lang)
{
    std::ifstream f("locale/" + lang + ".lang");
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
    std::ifstream f("locale/Keys.lk");
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