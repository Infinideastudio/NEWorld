// DataKit.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DataKit.h"
#include <stdio.h>
#include <sstream>

// 这是导出变量的一个示例
DATAKIT_API int nDataKit=0;

// 这是导出函数的一个示例。
DATAKIT_API int fnDataKit(void)
{
    return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 DataKit.h
CDataKit::CDataKit()
{
    return;
}

namespace DataKit
{
    void object::read(std::fstream & file)
    {
    }

    void object::write(std::fstream & file)
    {
    }

    object::~object()
    {
    }

    void cl_int::read(std::fstream & file)
    {
        file.read((char*)(&val), sizeof(long long));
    }

    void cl_int::write(std::fstream & file)
    {
        file.write((char*)(&val), sizeof(long long));
    }

    cl_int::cl_int()
    {
        type = DK_INT;
    }

    void cl_float::read(std::fstream & file)
    {
        file.read((char*)(&val), sizeof(double));
    }

    void cl_float::write(std::fstream & file)
    {
        file.write((char*)(&val), sizeof(double));
    }

    cl_float::cl_float()
    {
        type = DK_FLOAT;
    }

    void cl_str::read(std::fstream & file)
    {
        int length;
        file.read((char*)&length, sizeof(int));
        wchar_t* str = new wchar_t[length + 1];
        file.read((char*)str, sizeof(wchar_t)*(length));
        str[length] = L'\0';
        *val = std::wstring(str);
        delete[]str;
    }

    void cl_str::write(std::fstream & file)
    {
        int length = val->length();
        file.write((char*)&length, sizeof(int));
        file.write((char*)val->c_str(), sizeof(wchar_t)*(length));
    }

    cl_str::cl_str()
    {
        val = new std::wstring();
        type = DK_STRING;
    }

    cl_str::~cl_str()
    {
        delete val;
    }

    typedef std::map<unsigned long long, object*> _sd;

    void cl_sheet::read(std::fstream & file)
    {
        unsigned long long count;
        file.read((char*)&count, sizeof(unsigned long long));
        for (int i = 0; i < count; ++i) 
        {
            unsigned long long pos;
            object_type type;
            file.read((char*)&pos, sizeof(unsigned long long));
            file.read((char*)&type, sizeof(object_type));
            object* t = nullptr;
            switch (type)
            {
            case DK_INT:
                t = new cl_int();
                break;
            case DK_FLOAT:
                t = new cl_float();
                break;
            case DK_STRING:
                t = new cl_str();
                break;
            case DK_SHEET:
                t = new cl_sheet();
                break;
            case DK_LIST:
                t = new cl_list();
                break;
            case DK_BINARY:
                t = new cl_binary();
                break;
            default:
                break;
            }
            t->read(file);
            ((_sd*)val)->insert({ pos, t });
        }
    }

    void cl_sheet::write(std::fstream & file)
    {
        unsigned long long count = ((_sd*)val)->size();
        file.write((char*)&count, sizeof(unsigned long long));
        for (std::map<unsigned long long, object*>::iterator i = ((_sd*)val)->begin(); i != ((_sd*)val)->end(); i++)
        {
            file.write((char*)&i->first, sizeof(unsigned long long));
            file.write((char*)&i->second->type, sizeof(object_type));
            i->second->write(file);
        }
    }

    void cl_sheet::addsub(unsigned long x, unsigned long y, object_type oftype)
    {
        object* t = nullptr;
        switch (oftype)
        {
        case DK_INT:
            t = new cl_int();
            break;
        case DK_FLOAT:
            t = new cl_float();
            break;
        case DK_STRING:
            t = new cl_str();
            break;
        case DK_SHEET:
            t = new cl_sheet();
            break;
        case DK_LIST:
            t = new cl_list();
            break;
        case DK_BINARY:
            t = new cl_binary();
            break;
        default:
            break;
        }
        ((_sd*)val)->insert({ x * 0xFFFFFFFF + y, t });
    }

    void cl_sheet::deletesub(unsigned long x, unsigned long y)
    {
        std::map<unsigned long long, object*>::iterator i;
        i = ((_sd*)val)->find(x * 0xFFFFFFFF + y);
        if (i != ((_sd*)val)->end())
        {
            ((_sd*)val)->erase(i->first);
        }
    }

    object * cl_sheet::sub(unsigned long x, unsigned long y)
    {
        std::map<unsigned long long, object*>::iterator i;
        i = ((_sd*)val)->find(x * 0xFFFFFFFF + y);
        if (i != ((_sd*)val)->end())
        {
            return i->second;
        }
        return nullptr;
    }


    object * cl_sheet::sub(std::wstring key)
    {
        std::wstringstream str(key);
        int x, y;
        str >> x >> y;
        return sub(x, y);
        return nullptr;
    }

    object * cl_sheet::operator[](const std::wstring key)
    {
        std::size_t found = key.find(L"::");
        if (found != std::wstring::npos) {
            object* res = sub(key.substr(0, found));
            if (res) {
                switch (res->type)
                {
                case DK_LIST:
                    return (*(cl_list*)res)[key.substr(found + 2)];
                    break;
                case DK_SHEET:
                    return (*(cl_sheet*)res)[key.substr(found + 2)];
                    break;
                default:
                    break;
                }
            }
            else {
                return nullptr;
            }
        }
        else
        {
            object* res = sub(key);
            if (res) {
                return res;
            }
            else
            {
                return nullptr;
            }
        }
        return nullptr;
    }

    cl_sheet::cl_sheet()
    {
        val = new std::map<unsigned long long, object*>();
        type = DK_SHEET;
    }

    DataKit::cl_sheet::~cl_sheet()
    {
        for (std::map<unsigned long long, object*>::iterator i = ((_sd*)val)->begin(); i != ((_sd*)val)->end(); i++)
        {
            delete i->second;
        }
        delete (_sd*)val;
    }

    typedef std::map<std::wstring, object*> _st;
    void cl_list::read(std::fstream & file)
    {
        unsigned long long count;
        file.read((char*)&count, sizeof(unsigned long long));
        for (int i = 0; i < count; ++i)
        {
            std::wstring key;
            int length;
            file.read((char*)&length, sizeof(int));
            wchar_t* str = new wchar_t[length + 1];
            file.read((char*)str, sizeof(wchar_t)*(length));
            str[length] = L'\0';
            key = std::wstring(str);
            delete[]str;
            object_type type;
            file.read((char*)&type, sizeof(object_type));
            object* t = nullptr;
            switch (type)
            {
            case DK_INT:
                t = new cl_int();
                break;
            case DK_FLOAT:
                t = new cl_float();
                break;
            case DK_STRING:
                t = new cl_str();
                break;
            case DK_SHEET:
                t = new cl_sheet();
                break;
            case DK_LIST:
                t = new cl_list();
                break;
            case DK_BINARY:
                t = new cl_binary();
                break;
            default:
                break;
            }
            t->read(file);
            ((_st*)val)->insert({ key, t });
        }
    }

    void cl_list::write(std::fstream & file)
    {
        unsigned long long count = ((_st*)val)->size();
        file.write((char*)&count, sizeof(unsigned long long));
        for (std::map<std::wstring, object*>::iterator i = ((_st*)val)->begin(); i != ((_st*)val)->end(); i++)
        {
            int length = i->first.length();
            file.write((char*)&length, sizeof(int));
            file.write((char*)i->first.c_str(), sizeof(wchar_t)*(length));
            file.write((char*)&i->second->type, sizeof(object_type));
            i->second->write(file);
        }
    }

    void cl_list::addsub(std::wstring key, object_type oftype)
    {
        object* t = nullptr;
        switch (oftype)
        {
        case DK_INT:
            t = new cl_int();
            break;
        case DK_FLOAT:
            t = new cl_float();
            break;
        case DK_STRING:
            t = new cl_str();
            break;
        case DK_SHEET:
            t = new cl_sheet();
            break;
        case DK_LIST:
            t = new cl_list();
            break;
        case DK_BINARY:
            t = new cl_binary();
            break;
        default:
            break;
        }
        ((_st*)val)->insert({ key, t });
    }

    void cl_list::deletesub(std::wstring key)
    {
        std::map<std::wstring, object*>::iterator i;
        i = ((_st*)val)->find(key);
        if (i != ((_st*)val)->end())
        {
            ((_st*)val)->erase(i->first);
        }
    }

    void cl_list::createdir(std::wstring key)
    {
        std::size_t found = key.find(L"::");
        if (found != std::wstring::npos) {
            object* res = sub(key.substr(0, found));
            if (!res||(res->type!=DK_LIST)) {
                addsub(key.substr(0, found), DK_LIST);
                ((cl_list*)((*this)[key.substr(0, found)]))->createdir(key.substr(found + 2));
            }
        }
        else
        {
            object* res = sub(key);
            if (!res || (res->type != DK_LIST)) {
                addsub(key.substr(0, found), DK_LIST);
            }
        }
    }

    void cl_list::createstr(std::wstring key)
    {
        std::size_t found = key.find(L"::");
        if (found != std::wstring::npos) {
            object* res = sub(key.substr(0, found));
            if (res) {
                ((cl_list*)((*this)[key.substr(0, found)]))->createstr(key.substr(found + 2));
            }
        }
        else
        {
           addsub(key.substr(0, found), DK_STRING);
        }
    }

    object * cl_list::sub(std::wstring key)
    {
        std::map<std::wstring, object*>::iterator i;
        i = ((_st*)val)->find(key);
        if (i != ((_st*)val)->end())
        {
            return i->second;
        }
        return nullptr;
    }

    object * cl_list::operator[](const std::wstring key)
    {
        std::size_t found = key.find(L"::");
        if (found != std::wstring::npos) {
            object* res = sub(key.substr(0, found));
            if (res) {
                switch (res->type)
                {
                case DK_LIST:
                    return (*(cl_list*)res)[key.substr(found + 2)];
                    break;
                case DK_SHEET:
                    return (*(cl_sheet*)res)[key.substr(found + 2)];
                    break;
                default:
                    break;
                }
            }
            else {
                return nullptr;
            }
        }
        else
        {
            object* res = sub(key);
            if (res) {
                return res;
            }
            else
            {
                return nullptr;
            }
        }
        return nullptr;
    }

    cl_list::cl_list()
    {
        val = new std::map<std::wstring, object*>();
        type = DK_LIST;
    }

    DataKit::cl_list::~cl_list()
    {
        for (std::map<std::wstring, object*>::iterator i = ((_st*)val)->begin(); i != ((_st*)val)->end(); i++)
        {
            delete i->second;
        }
        delete (_st*)val;
    }

    void cl_binary::read(std::fstream & file)
    {
        file.read((char*)&size, sizeof(int));
        val = new char[size];
        file.read((char*)val, size);
    }

    void cl_binary::write(std::fstream & file)
    {
        file.write((char*)&size, sizeof(int));
        file.write((char*)val, size);
    }

    cl_binary::cl_binary()
    {
        type = DK_BINARY;
    }

    cl_binary::~cl_binary()
    {
        delete[](char*)val;
    }

    void cl_file::open(std::wstring name)
    {
        fname = name;
        std::fstream f(name, std::ios::binary | std::ios::in);
        if (f.good()) {
            read(f);
            f.close();
        }
    }

    void cl_file::save()
    {
        std::fstream f(fname, std::ios::binary | std::ios::out);
        write(f);
        f.close();
    }
}
