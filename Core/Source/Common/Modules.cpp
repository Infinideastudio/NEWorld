// 
// Core: Modules.cpp
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

#include "Modules.h"
#include "Common/Dylib.h"
#include "Common/Logger.h"
#include "Common/Filesystem.h"
#include "Common/JsonHelper.h"
#include <cstdlib>
#include <set>

struct Version {
    constexpr Version(int a, int b, int c, int d) : vMajor(a), vMinor(b), vRevision(c), vBuild(d) {}
    int vMajor, vMinor, vRevision, vBuild;
};

constexpr bool operator >=(const Version& l, const Version& r) noexcept {
    if (l.vMajor < r.vMajor) return false;
    if (l.vMajor == r.vMajor && l.vMinor < r.vMinor) return false;
    if (l.vMinor == r.vMinor && l.vRevision < r.vRevision) return false;
    if (l.vRevision == r.vRevision && l.vBuild < r.vBuild) return false;
    return true;
}

constexpr bool operator <=(const Version& l, const Version& r) noexcept {
    if (l.vMajor > r.vMajor) return false;
    if (l.vMajor == r.vMajor && l.vMinor > r.vMinor) return false;
    if (l.vMinor == r.vMinor && l.vRevision > r.vRevision) return false;
    if (l.vRevision == r.vRevision && l.vBuild > r.vBuild) return false;
    return true;
}

constexpr bool operator ==(const Version& l, const Version& r) noexcept {
    return (l.vMajor == r.vMajor) && (l.vMinor == r.vMinor) && (l.vRevision == r.vRevision) && (l.vBuild == r.vBuild);
}

constexpr bool operator <(const Version& l, const Version& r) noexcept { return !(l >= r); }

constexpr bool operator >(const Version& l, const Version& r) noexcept { return !(l <= r); }

class Module {
public:
    Module(Library lib, std::unique_ptr<ModuleObject> obj) :
        mLib(std::move(lib)), mObject(std::move(obj)){}
    Module(Module&&) = default;
    Module& operator =(Module&&) = default;
    Module(const Module&) = delete;
    Module& operator =(const Module&) = delete;
    ~Module() {
        mObject.release();
        mLib.unload();
    }
private:
    Library mLib;
    std::unique_ptr<ModuleObject> mObject;
};

struct Modules {
    std::vector<Module> modules;
    std::set<std::string> nameMap;
};

// Plugin system
class ModuleManager final {
public:
    ModuleManager(ModuleManager&&) = delete;
    ModuleManager& operator =(ModuleManager&&) = delete;
    ModuleManager(const ModuleManager&) = delete;
    ModuleManager& operator =(const ModuleManager&) = delete;
    void load();
    size_t getCount() const noexcept { return mModules.nameMap.size(); }
    bool isLoaded(const std::string& uri) const { return mModules.nameMap.find(uri) != mModules.nameMap.end(); }
    static auto& getInstance() {
        static ModuleManager mgr;
        return mgr;
    }
private:
    ModuleManager();
    ~ModuleManager();
    class ModuleLoader;
    Modules mModules;
};

void loadModules() { ModuleManager::getInstance().load(); }

bool isModuleLoaded(const std::string& uri) { return ModuleManager::getInstance().isLoaded(uri); }

int getModuleCount() noexcept { return ModuleManager::getInstance().getCount(); }

///////////////////////////////////////////////////////////////////////////////
//                            IMPLEMENTATION
///////////////////////////////////////////////////////////////////////////////

class ModuleManager::ModuleLoader final {
    struct DependencyInfo {
        std::string uri;
        Version vRequired{ 0,0,0,0 };
        bool isOptional = false;
    };

    using DependencyList = std::vector<DependencyInfo>;

    struct ModuleInfo {
        std::string name, author, uri;
        Version thisVersion{ 0,0,0,0 }, conflictVersion{ 0,0,0,0 };
        DependencyList dependencies;
    };

    enum class Status {
        Pending,
        Success,
        Fail
    };

    struct LoadingInfo {
        ModuleInfo info;
        Library lib;
        Status stat = Status::Pending;
    };

    static auto getModuleDir() { return executablePath() / "Modules"; }

public:
    ModuleLoader();
    auto&& result() && { return std::move(mResult); }
private:
    void loadPlugin(const std::string& uri);
    void loadPlugin(LoadingInfo& inf) noexcept;
    void verify(const DependencyInfo& inf);
    void walk();

    static Version extractVersion(Json & json);
    static ModuleInfo extractInfo(const char* json);

    Modules mResult;
    std::unordered_map<std::string, LoadingInfo> mMap;
};

ModuleManager::ModuleLoader::ModuleLoader() {
    constexpr auto pathSep =
#if (BOOST_OS_WINDOWS)
        ";";
#else
        ":";
#endif
    std::string env = std::getenv("PATH");
    env = "PATH=" + env + pathSep + filesystem::absolute(getModuleDir()).string() + pathSep;
    auto const nenv = new char[env.size() + 1];
    std::strcpy(nenv, env.c_str());
#if BOOST_COMP_MSVC
    _putenv(nenv);
#else
    putenv(nenv);
#endif
    walk();
    for (auto&& x : mMap)
        loadPlugin(x.second);
}

void ModuleManager::ModuleLoader::loadPlugin(const std::string& uri) { loadPlugin(mMap[uri]); }

void ModuleManager::ModuleLoader::loadPlugin(LoadingInfo& inf) noexcept {
    if (inf.stat != Status::Pending) return;
    try {
        infostream << "Loading Module: " << inf.info.uri;
        for (auto& x : inf.info.dependencies) {
            loadPlugin(x.uri);
            try { verify(x); }
            catch (std::exception& e) {
                warningstream << "Module Denpendency " << x.uri << " Of: " << inf.info.uri <<
                    " Failed For: " << e.what();
                if (x.isOptional)
                    warningstream << "Dependency Skipped For It Is Optional";
                else
                    throw;
            }
        }
        std::unique_ptr<ModuleObject> object;
        if (const auto getObject = inf.lib.get<ModuleObject*()>("nwModuleGetObject"); getObject)
            object.reset(getObject());
        else
            throw std::runtime_error("Module has no nwModuleGetObject function, skipping finalization!");
        // No Error, Load Success
        inf.stat = Status::Success;
        mResult.nameMap.insert(inf.info.uri);
        mResult.modules.emplace_back(std::move(inf.lib), std::move(object));
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

void ModuleManager::ModuleLoader::verify(const DependencyInfo& inf) {
    auto& depStat = mMap[inf.uri];
    if (depStat.stat != Status::Success) throw std::runtime_error("Dependency Load Failure");
    if (inf.vRequired > depStat.info.thisVersion)
        throw std::runtime_error(
            "Dependency Version Mismatch : Version Unsupported");
    if (inf.vRequired < depStat.info.conflictVersion)
        warningstream << "Dependency Version Mismatch : Confilict. Module May Behave Strangely. Use with care";
}

void ModuleManager::ModuleLoader::walk() {
    infostream << "Start Walking Module Dir...";
    const auto path = getModuleDir();
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
                        throw std::runtime_error(
                            "Module:" + file.path().filename().string() +
                            " lacks required function: const char* nwModuleGetInfo()");
                    info.stat = Status::Pending;
                    mMap.emplace(info.info.uri, std::move(info));
                }
                catch (std::exception& e) { warningstream << e.what(); }
            }
        };
    }
    infostream << mMap.size() << " Modules(s) Founded";
}

Version ModuleManager::ModuleLoader::extractVersion(Json& json) {
    auto ver = getJsonValue<std::vector<int>>(json);
    return {
        !ver.empty() ? ver[0] : 0, ver.size() >= 2 ? ver[1] : 0,
        ver.size() >= 3 ? ver[2] : 0, ver.size() >= 4 ? ver[3] : 0
    };
}

ModuleManager::ModuleLoader::ModuleInfo ModuleManager::ModuleLoader::extractInfo(const char* json) {
    ModuleInfo ret;
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

ModuleManager::ModuleManager() = default;

void ModuleManager::load(){
    infostream << "Start to load plugins...";
    mModules = std::move(ModuleLoader()).result();  
}

ModuleManager::~ModuleManager() {
    while (!mModules.modules.empty()) {
        mModules.modules.pop_back();
    }
}
