// 
// Core: Dylib.h
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

#pragma once

#include "Config.h"
#include <string>

class NWCOREAPI Library {
public:
    using HandleType = void*;

    Library() = default;

    explicit Library(const std::string& filename);

    Library(Library&& library) noexcept;

    Library& operator=(Library&& library) noexcept;

    Library(const Library& library) = delete;

    Library& operator=(const Library& library) = delete;

    ~Library();

    template <class T>
    auto get(const std::string& name) const { return get<T>(name.c_str()); }

    template <class T>
    auto get(const char* name) const { return reinterpret_cast<std::decay_t<T>>(getFuncImpl(mDllHandle, name)); }

    explicit operator bool() const noexcept { return isLoaded(); }

    bool isLoaded() const noexcept { return mLoaded; }

    void load(const std::string& filename);

    void unload();

private:
    using FcnProcAddr = void(*)();
    static FcnProcAddr getFuncImpl(HandleType handle, const std::string& name);
    HandleType mDllHandle = nullptr;
    bool mLoaded = false;
};
