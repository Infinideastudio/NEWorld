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
#include "Common/Json/JsonHelper.h"

#if (BOOST_OS_CYGWIN || BOOST_OS_WINDOWS)
#include "Common/Internals/Windows.hpp"
#else
#include <stdlib.h>
#endif


struct Version {
    int vMajor, vMinor, vRevision, vBuild;
};

constexpr bool operator >= (const Version& l, const Version& r) noexcept {
    if (l.vMajor < r.vMajor) return false;
    if (l.vMajor == r.vMajor && l.vMinor < r.vMinor) return false;
    if (l.vMinor == r.vMinor && l.vRevision < r.vRevision) return false;
    if (l.vRevision == r.vRevision && l.vBuild < r.vBuild) return false;
    return true;
}

constexpr bool operator <= (const Version& l, const Version& r) noexcept {
    if (l.vMajor > r.vMajor) return false;
    if (l.vMajor == r.vMajor && l.vMinor > r.vMinor) return false;
    if (l.vMinor == r.vMinor && l.vRevision > r.vRevision) return false;
    if (l.vRevision == r.vRevision && l.vBuild > r.vBuild) return false;
    return true;
}

constexpr bool operator == (const Version& l, const Version& r) noexcept {
    return (l.vMajor == r.vMajor) && (l.vMajor == r.vMajor) && (l.vMinor == r.vMinor) && (l.vRevision == r.vRevision);
}

constexpr bool operator < (const Version& l, const Version& r) noexcept {
    return !(l >= r);
}

constexpr bool operator > (const Version& l, const Version& r) noexcept {
    return !(l <= r);
}

struct DependencyInfo {
    std::string uri;
    Version vRequired;
    bool isOptional;
};

using DependencyList = std::vector<DependencyInfo>;

struct PluginInfo {
    std::string name, author, uri;
    Version thisVersion, conflictVersion;
    DependencyList dependencies;
};

class DefaultPluginObject : public PluginObject {
public:
    DefaultPluginObject(Library& lib) : mLib(lib) {
        const auto init = lib.get<void NWAPICALL()>("nwModuleInitialize");
        if (init)
            init();
        else
            infostream << "Module Has no init function, skipping initialization!";
    }
    
    ~DefaultPluginObject() {
        const auto finalize = mLib.get<void NWAPICALL()>("nwModuleFinalize");
        if (finalize)
            finalize();
        else
            infostream << "Module Has no unload function, skipping finalization!";
    }
private:
    Library& mLib;
};

class PluginLoader {
    enum class Status {
        Pending,
        Success,
        Fail
    };

    struct LoadingInfo {
        PluginInfo info;
        Library lib;
        Status stat;
    };

public:
    PluginLoader() {
		constexpr const char* pathSep =
#if (BOOST_OS_WINDOWS)
			";";
#else
			":";
#endif
		std::string env = std::getenv("PATH");
		env += pathSep + filesystem::absolute("./Modules/").string() + pathSep;
#if (BOOST_OS_CYGWIN || BOOST_OS_WINDOWS)
		env = "PATH=" + env;
		char* nenv = new char[env.size() + 1];
		std::strcpy(nenv, env.c_str());
		putenv(nenv);
#else
		setenv("PATH", env.c_str(), 1);
#endif
        walk();
        for (auto&& x : mMap)
            loadPlugin(x.second);
    }

    void loadPlugin(const std::string uri) { loadPlugin(mMap[uri]); }

    void loadPlugin(LoadingInfo& inf) noexcept {
        if (inf.stat != Status::Pending) return;
        try {
            infostream << "Loading Module: " << inf.info.uri;
            for (auto& x : inf.info.dependencies) {
                loadPlugin(x.uri);
                try {
                    verify(x);
                }
                catch (std::exception& e) {
                    warningstream << "Module Denpendency " << x.uri << " Of: " << inf.info.uri <<
                        " Failed For: " << e.what();
                    if (x.isOptional)
                        warningstream << "Dependency Skipped For It Is Optional";
                    else
                        throw;
                }
            }
            auto object = std::make_unique<DefaultPluginObject>(inf.lib);
            // No Error, Load Success
            inf.stat = Status::Success;
            mResult[inf.info.uri].lib = std::move(inf.lib);
            mResult[inf.info.uri].object = std::move(object);
        }
        catch (std::exception& e) {
            warningstream << "Module: " << inf.info.uri << " Failed For: " << e.what();
            inf.stat = Status::Fail;
        }
        catch (...) {
            warningstream << "Module: " << inf.info.uri << " Failed For Unknown Reason";
            inf.stat = Status::Fail;
        }
    }

    void verify(DependencyInfo& inf) {
        auto& depStat = mMap[inf.uri];
        if (depStat.stat != Status::Success) throw std::runtime_error("Dependency Load Failure");
        if (inf.vRequired > depStat.info.thisVersion) throw std::runtime_error("Dependency Version Mismatch : Version Unsupported");
        if (inf.vRequired < depStat.info.conflictVersion)
            warningstream << "Dependency Version Mismatch : Confilict. Module May Behave Strangely. Use with care";
    }

    void walk() {
        infostream << "Start Walking Module Dir...";
        const filesystem::path path = "./Modules/";
        if (filesystem::exists(path)) {
            for (auto&& file : filesystem::directory_iterator(path)) {
                if (file.path().extension().string() == ".nwModule") {
                    try {
                        LoadingInfo info;
                        info.lib.load(file.path().string());
                        const auto infoFunc = info.lib.get<const char* NWAPICALL()>("nwModuleGetInfo");
                        if (infoFunc)
                            info.info = extractInfo(infoFunc());
                        else
                            throw std::runtime_error("Module:" + file.path().filename().string() + " lacks required function: const char* nwModuleGetInfo()");
                        info.stat = Status::Pending;
                        mMap.emplace(info.info.uri, std::move(info));
                    }
                    catch (std::exception& e) {
                        warningstream << e.what();
                    }
                }
            };
        }
        infostream << mMap.size() << " Modules(s) Founded";
    }

    Version extractVersion(Json& json) {
        auto ver = getJsonValue<std::vector<int>>(json);
        return { ver.size() >= 1 ? ver[0] : 0, ver.size() >= 2 ? ver[1] : 0,
            ver.size() >= 3 ? ver[2] : 0, ver.size() >= 4 ? ver[3] : 0 };
    }

    PluginInfo extractInfo(const char* json) {
        PluginInfo ret;
        auto js = Json::parse(json);
        ret.author = getJsonValue<std::string>(js["author"]);
        ret.name = getJsonValue<std::string>(js["name"]);
        ret.uri = getJsonValue<std::string>(js["uri"]);
        ret.thisVersion = extractVersion(js["version"]);
        ret.conflictVersion = extractVersion(js["conflictVersion"]);
        if (auto& deps = js["dependencies"]; !deps.is_null()) {
            for (auto&& x : deps) {
                DependencyInfo info;
                info.uri = getJsonValue<std::string>(x["uri"]);
                info.vRequired = extractVersion(x["required"]);
                info.isOptional = getJsonValue<bool>(x["optional"], false);
                ret.dependencies.push_back(std::move(info));
            }
        }
        return ret;
    }

    auto&& result()&& { return std::move(mResult); }
private:
    std::unordered_map<std::string, LoadingInfo> mMap;
    std::map<std::string, PluginPair> mResult;
};

PluginManager::PluginManager() {
    infostream << "Start to load plugins...";
    mPlugins = std::move(PluginLoader()).result();
}

PluginManager::~PluginManager() = default;
