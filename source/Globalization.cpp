#include "Globalization.h"
#include "../include/sqlite/sqlite3.h"

namespace Globalization
{
<<<<<<< HEAD

int count;
string CurLang = "zh_CN";    //���������Ա�׼����
map<string, string> Lines;   //�����˵�ǰ�����ص�����
sqlite3 *db = nullptr;       //���ݿ�

void LoadLang(string lang)
{
    CurLang = lang;
	char *zErrMsg = nullptr;
	int nrow = 0, ncolumn = 0; //��ѯ�����������������
	char **azResult;           //��ά�����Ž��
	sqlite3_get_table(db, ("SELECT key," + CurLang + " FROM main ").c_str(), &azResult, &nrow, &ncolumn, &zErrMsg);//��ѯ
	Lines.clear();             //�������
	for (int i = 1; i < nrow + 1; ++i) 
		Lines[azResult[i * 2]] = azResult[i * 2 + 1]; //д���ֵ
    sqlite3_free_table(azResult);
}

void Load()
{
	//�����������ݿ�
	if (sqlite3_open("res/local.db", &db))
	{
		throw - 1;
		sqlite3_close(db);
	}
	LoadLang(CurLang);
}

string GetStr(string key)
{
    return Lines[key];
}

vector<LangInfo> GetLangs()
{
    char *zErrMsg = nullptr;
    int nrow = 0, ncolumn = 0; //��ѯ�����������������
    char **azResult;           //��ά�����Ž��
    sqlite3_get_table(db, "SELECT * FROM main LIMIT 0,3", &azResult, &nrow, &ncolumn, &zErrMsg);//��ѯ
    vector<LangInfo> res;
    for (int i = 1; i < ncolumn; ++i) 
    {
        res.push_back({ azResult[i], azResult[ncolumn + i], azResult[ncolumn * 2 + i] });
    }
    sqlite3_free_table(azResult);
    return res;
}

string & Cur_Lang()
{
    return CurLang;
}

void Finalize()
{
	sqlite3_close(db);
=======

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
>>>>>>> 0.5.0
}
}