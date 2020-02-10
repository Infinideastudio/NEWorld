// 
// Core: Dylib.cpp
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

#include "Dylib.h"
#include <cassert>
#include <sstream>

#if (__CYGWIN__ || _WIN32)

#include "Internals/Windows.hpp"

namespace {
    Library::HandleType loadLibrary(const std::string& filename, bool& success) {
        const auto handle = LoadLibraryA(filename.c_str());
        success = handle != nullptr;
        if (!success) {
            std::stringstream ss;
            ss << "Failed to load " << filename << ". Error code:" << GetLastError();
            throw std::runtime_error(ss.str());
        }
        return handle;
    }

    void freeLibrary(Library::HandleType handle) { FreeLibrary(reinterpret_cast<HMODULE>(handle)); }
}

Library::FcnProcAddr Library::getFuncImpl(Library::HandleType handle, const std::string& name) {
    assert(handle != nullptr);
    return reinterpret_cast<FcnProcAddr>(GetProcAddress(reinterpret_cast<HMODULE>(handle), name.c_str()));
}

#else

#include <dlfcn.h>

namespace {
    Library::HandleType loadLibrary(const std::string& filename, bool& success) {
        const auto handle = dlopen(filename.c_str(), RTLD_NOW);
        if (handle == nullptr)
            throw std::runtime_error(dlerror());
        success = handle != nullptr;
        return handle;
    }

    void freeLibrary(Library::HandleType handle) {
        dlclose(handle);
    }
}

Library::FcnProcAddr Library::getFuncImpl(Library::HandleType handle, const std::string& name) {
    assert(handle != nullptr);
    return reinterpret_cast<FcnProcAddr>(dlsym(handle, name.c_str()));
}

#endif


Library::Library(const std::string& filename) : Library {} { load(filename); }

Library::Library(Library&& library) noexcept : Library {} {
    std::swap(library.mDllHandle, mDllHandle);
    std::swap(library.mLoaded, mLoaded);
}

Library& Library::operator=(Library&& library) noexcept {
    std::swap(library.mDllHandle, mDllHandle);
    std::swap(library.mLoaded, mLoaded);
    return *this;
}

Library::~Library() {
    if (isLoaded())
        freeLibrary(mDllHandle);
}

void Library::load(const std::string& filename) {
    if (isLoaded())
        unload();
    mDllHandle = loadLibrary(filename, mLoaded);
}

void Library::unload() {
    freeLibrary(mDllHandle);
    mLoaded = false;
}
