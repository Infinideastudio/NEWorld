// 
// nwcore: pluginmanager.cpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#include "pluginmanager.h"
#include "Common/StringUtils.h"
#include "Common/Filesystem.h"
#include "Common/Logger.h"

struct Version
{
    int vMajor, vMinor, vRevision, vBuild;
};

struct DependencyInfo
{
    std::string uri;
    Version vRequired;
    bool isOptional;
};

using DependencyList = std::vector<DependencyInfo>;

struct PluginInfo
{
    std::string name, author, uri;
    Version thisVersion, conflictVersion;
    DependencyList dependencies;
};


class PluginLoader
{
public:
    PluginLoader()
    {
    }

    void walk()
    {
        infostream << "Start Walking Module Dir...";
        const filesystem::path path = "./Modules/";
        if (filesystem::exists(path))
        {
            for (auto&& file : filesystem::directory_iterator(path))
            {
                auto suffix = file.path().extension().string();
                strToLower(suffix);
                if (suffix == ".nwModule")
                {
                    try
                    {
                        LoadingInfo info;
                        info.lib.load(file.path().string());
                        auto infoFunc = info.lib.get<const char*()>("nwModuleGetInfo");
                        if (infoFunc)
                            info.info = extractInfo(infoFunc());
                        else
                            throw std::runtime_error("Module lacks required function: const char* nwModuleGetInfo()");

                    }
                    catch(std::exception&)
                    {
                        warningstream << 
                    }   
                }
            };
        }
        infostream << mMap.size() << " Modules(s) Founded";
    }

    PluginInfo extractInfo(const char* json)
    {
        
    }

private:
    enum class Status
    {
        Pending,
        Success,
        Fail
    };

    struct LoadingInfo
    {
        PluginInfo info;
        Library lib;
        Status stat;
    };

    std::unordered_map<std::string, LoadingInfo> mMap;
};

PluginManager::PluginManager()
{
    infostream << "Start to load plugins...";
    size_t counter = 0;
    const filesystem::path path = "./Modules/";
    if (filesystem::exists(path))
    {
        for (auto&& file : filesystem::directory_iterator(path))
        {
            auto suffix = file.path().extension().string();
            strToLower(suffix);
            if (suffix == LibSuffix)
            {
                debugstream << "Loading:" << file.path().string();
                if (loadPlugin(file.path().string()))
                    counter++;
            }
        };
    }
    infostream << counter << " plugin(s) loaded";
}

PluginManager::~PluginManager() { mPlugins.clear(); }

bool PluginManager::loadPlugin(const std::string& filename)
{
    auto& plugin = mPlugins.emplace_back(filename);

    if (!plugin.isLoaded())
    {
        mPlugins.pop_back();
        warningstream << "Failed to load plugin from \"" << filename << "\", skipping";
        return false;
    }
    infostream << "Loaded plugin \"" << plugin.getData().pluginName << "\"["
        << plugin.getData().internalName
        << "], authored by \"" << plugin.getData().authorName << "\"";
    plugin.init(nwPluginTypeCore);
    return true;
}
