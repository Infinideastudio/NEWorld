#include "Globalization.h"
#include "../include/sqlite/sqlite3.h"

namespace Globalization
{

int count;
string CurLang = "zh_CN";    //代表了语言标准名称
map<string, string> Lines;   //储存了当前被加载的语言
sqlite3 *db = nullptr;       //数据库

void LoadLang(string lang)
{
    CurLang = lang;
	char *zErrMsg = nullptr;
	int nrow = 0, ncolumn = 0; //查询结果集的行数、列数
	char **azResult;           //二维数组存放结果
	sqlite3_get_table(db, ("SELECT key," + CurLang + " FROM main ").c_str(), &azResult, &nrow, &ncolumn, &zErrMsg);//查询
	Lines.clear();             //清空数据
	for (int i = 1; i < nrow + 1; ++i) 
		Lines[azResult[i * 2]] = azResult[i * 2 + 1]; //写入键值
    sqlite3_free_table(azResult);
}

void Load()
{
	//挂载语言数据库
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
    int nrow = 0, ncolumn = 0; //查询结果集的行数、列数
    char **azResult;           //二维数组存放结果
    sqlite3_get_table(db, "SELECT * FROM main LIMIT 0,3", &azResult, &nrow, &ncolumn, &zErrMsg);//查询
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
}
}