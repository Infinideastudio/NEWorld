#include "ModLoader.h"
#include <Windows.h>
#include <fstream>
#include <io.h>
#include <functional>
#include <iostream>
#include "API.h"

std::vector<ModInfo> Mod::ModLoader::mods;

//查找一个文件夹下所有的子目录
std::vector<std::string> findFolders(std::string path)
{
    std::vector<std::string> ret;
    long hFile = 0;
    _finddata_t fileinfo;
    if ((hFile = _findfirst((path+"*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            if ((fileinfo.attrib &  _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                {
                    ret.push_back(fileinfo.name);
                }
            }
        }
        while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
    return ret;
}

void Mod::ModLoader::loadMods()
{
    std::vector<std::string> modLoadList = findFolders("Mods/"); //获得Mods/下所有的文件夹
    bool successAtLeastOne = false;
    do   //循环加载Mod，直到某一次循环一个成功加载的都没有为止（因为依赖项）
    {
        successAtLeastOne = false;
        for (std::vector<std::string>::iterator iter = modLoadList.begin(); iter != modLoadList.end();)
        {
            ModLoadStatus status = loadSingleMod("Mods/" + *iter + "/entry.dll");
            if (status == Success) successAtLeastOne = true;
            if (status != MissDependence) iter = modLoadList.erase(iter); //只要不是因缺少依赖而失败就删掉记录
            else ++iter;
        }
    }
    while (successAtLeastOne);
    std::cout << "[Console][Game]Load complete! "
              << mods.size() << " mod" << (mods.size() > 1 ? "s" : "") << " loaded." << std::endl;
}

//加载单个Mod，loadMods会调用该函数
Mod::ModLoader::ModLoadStatus Mod::ModLoader::loadSingleMod(std::string modPath)
{
    ModCall call=loadMod(modPath);
    ModInfo(*getModInfo)() = (ModInfo(*)())getFunction(call, "getModInfo");
    bool(*init)(APIPackage) = (bool(*)(APIPackage))getFunction(call, "init");
    ModInfo info = getModInfo(); //获得Mod信息
    std::cout << "[Console][Game]Loading Mod " << info.name << "[" << info.version << "]" << std::endl;
    if (strcmp(info.dependence, "") != 0)   //判断并检查依赖项
    {
        bool foundDependence = false;
        for (std::vector<ModInfo>::iterator iter = mods.begin(); iter != mods.end(); ++iter)
        {
            if (iter->name == info.dependence)
            {
                foundDependence = true;
                break;
            }
        }
        if (!foundDependence) return MissDependence;
    }
    if (!(*init)(getPackage())) return InitFailed; //初始化Mod
    info.call = call;
    mods.push_back(info);
    return Success;
}

void Mod::ModLoader::unloadMods()
{
    for (size_t i = 0; i != mods.size(); i++)
    {
        unloadMod(mods[i].call);
    }
    mods.clear();
}


Mod::ModLoader::ModCall Mod::ModLoader::loadMod(std::string filename)
{
    return LoadLibrary(filename.c_str());
}

Mod::ModLoader::FuncPtr Mod::ModLoader::getFunction(ModCall call, std::string functionName)
{
    return GetProcAddress((HMODULE)call, functionName.c_str());
}

void Mod::ModLoader::unloadMod(ModCall call)
{
    FreeLibrary((HMODULE)call);
}
