// 
// Core: JsonHelper.h
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

#include "nlohmann/json.hpp"
#include "Common/Config.h"

using Json = nlohmann::json;

const std::string SettingsFilename = "./settings";

NWCOREAPI Json readJsonFromFile(std::string filename);

NWCOREAPI void writeJsonToFile(std::string filename, Json& json);

NWCOREAPI Json& getSettings();

// get a json value. If it does not exist, return the default value and write it to the json
template <class T>
T getJsonValue(Json& json, T defaultValue = T()) {
    if (json.is_null()) {
        json = defaultValue;
        return defaultValue;
    }
    return json;
}
