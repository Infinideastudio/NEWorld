// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 DATAKIT_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// DATAKIT_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef DATAKIT_EXPORTS
#define DATAKIT_API __declspec(dllexport)
#else
#define DATAKIT_API __declspec(dllimport)
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <map>

// 此类是从 DataKit.dll 导出的
class DATAKIT_API CDataKit {
public:
	CDataKit(void);
	// TODO:  在此添加您的方法。
};

namespace DataKit
{
    enum object_type
    {
        DK_INT, DK_FLOAT, DK_STRING, DK_SHEET, DK_LIST, DK_BINARY
    };

    class DATAKIT_API object {
    public:
        object_type type;
        virtual void read(std::fstream & file);
        virtual void write(std::fstream & file);
        virtual ~object();
    };

    class DATAKIT_API cl_int : public object {
    public:
        long long val;
        void read(std::fstream & file);
        void write(std::fstream & file);
        cl_int();
    };

    class DATAKIT_API cl_float : public object {
    public:
        double val;
        void read(std::fstream & file);
        void write(std::fstream & file);
        cl_float();
    };

    class DATAKIT_API cl_str : public object {
    public:
        std::wstring* val;
        void read(std::fstream & file);
        void write(std::fstream & file);
        cl_str();
        ~cl_str();
    };

    class DATAKIT_API cl_sheet : public object {
    public:
        void* val;
        void read(std::fstream & file);
        void write(std::fstream & file);
        void addsub(unsigned long x, unsigned long y, object_type oftype);
        void deletesub(unsigned long x, unsigned long y);
        object* sub(unsigned long x, unsigned long y);;
        object* sub(std::wstring key);
        object* operator [](const std::wstring key);
        cl_sheet();
        ~cl_sheet();
    };

    class DATAKIT_API cl_list : public object {
    public:
        void *val;
        void read(std::fstream & file);
        void write(std::fstream & file);
        void addsub(std::wstring key, object_type oftype);
        void deletesub(std::wstring key);
        object* sub(std::wstring key);
        object* operator [](const std::wstring key);
        cl_list();
        ~cl_list();
    };


    class DATAKIT_API cl_binary : public object {
    public:
        void *val;
        int size;
        void read(std::fstream & file);
        void write(std::fstream & file);
        cl_binary();
        ~cl_binary();
    };

	class DATAKIT_API cl_file : public cl_list {
		std::wstring fname;
	public:
		void open(std::wstring name);
		void save();
	};

}

extern DATAKIT_API int nDataKit;

DATAKIT_API int fnDataKit(void);
