// LangFileGenerator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "../../DataKit/DataKit.h"
#include <fstream>
using namespace std;
using namespace DataKit;
int main()
{
    wfstream i1("Langs.txt", ios::in);
    cl_file keyfile;
    keyfile.open(L"Keys.dksf");
    int count;
    i1 >> count;
    i1.get();
    for (int i = 0; i < count; ++i) {
        wstring t;
        getline(i1, t);
        keyfile.createdir(t);
        ((cl_list*)keyfile[t])->addsub(L"Symbol", DK_STRING);
        ((cl_list*)keyfile[t])->addsub(L"Name", DK_STRING);
        getline(i1, t);
        *((cl_str*)keyfile[t + L"::Symbol"])->val = t;
        getline(i1, t);
        *((cl_str*)keyfile[t + L"::Name"])->val = t;
        int dtrc;
        wfstream i2(t + L".lang", ios::in);
        wfstream i3("Keys.lk", ios::in);
        i3 >> dtrc;
        i3.get();
        cl_file lang;
        lang.open(t + L".dksf");
        for (int j = 0; j < dtrc; ++j) {
            wstring t2;
            getline(i3, t2);
            lang.createdir(t2);
        }

        i3 >> dtrc;
        i3.get();
        for (int j = 0; j < dtrc; ++j) {
            wstring t2, t3;
            getline(i3, t2);
            lang.createstr(t2);
            getline(i2, t3);
            *((cl_str*)keyfile[t2])->val = t3;
        }
        lang.save();
    }
    return 0;
}

