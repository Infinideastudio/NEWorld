// 
// nwcore: plugin.cpp
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

#include "plugin.h"
#include "Common/Logger.h"

void Plugin::loadFrom(const std::string& filename) {
    GetInfoFunction* getinfo = nullptr;
    mLib.load(filename);
    if (mLib.isLoaded()) {
        try {
            getinfo = mLib.get<GetInfoFunction>("getInfo");
            if (getinfo)
                mData = getinfo();
            else {
                warningstream << "Lack of Info Func!";
                return mStatus = 2;
            }
        }
        catch (std::exception& e) { warningstream << "Failed: unhandled exception: " << e.what(); }
    }
    else
        return mStatus = 1; // Failed: could not load
    return mStatus = 0;
}

void Plugin::unload() {
    if (mStatus != 0) return;
    mStatus = -1;
    UnloadFunction* unload = nullptr;
    if (mLib.isLoaded()) {
        try {
            unload = mLib.get<UnloadFunction>("unload");
            if (unload)
                unload();
            else {
                // Warning: entry not found
                warningstream << "Subroutine unload() not found in plugin " << mData->internalName
                    << ", skipped unloading!";
            }
            mLib.unload();
        }
        catch (std::exception& e) { warningstream << "Failed: unhandled exception: " << e.what(); }
    }
}
