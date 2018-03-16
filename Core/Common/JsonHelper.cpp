// 
// Core: JsonHelper.cpp
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

#include "JsonHelper.h"
#include "Common/Logger.h"
#include <fstream>

namespace {
    class JsonSaveHelper {
    public:
        JsonSaveHelper(Json& json, std::string filename) :
            mJson(json), mFilename(std::move(filename)) {}

        JsonSaveHelper(const JsonSaveHelper&) = delete;
        JsonSaveHelper& operator=(const JsonSaveHelper&) = delete;
        ~JsonSaveHelper() { writeJsonToFile(mFilename, mJson); }
    private:
        Json& mJson;
        std::string mFilename;
    };

}

NWCOREAPI Json readJsonFromFile(std::string filename) {
    std::ifstream file(filename);
    if (file) {
        std::string content = std::string(std::istreambuf_iterator<char>(file),
                                          std::istreambuf_iterator<char>());
        if (!content.empty()) {
            try { return Json::parse(content); }
            catch (std::invalid_argument& exception) {
                warningstream << "Failed to load json " << filename << ": " << exception.what();
            }
        }
    }
    return Json();
}

NWCOREAPI void writeJsonToFile(std::string filename, Json& json) {
    const std::string& dump = json.dump();
    if (!json.is_null())
        std::ofstream(filename).write(dump.c_str(), dump.length());
}

NWCOREAPI Json& getSettings() {
    static Json settings = readJsonFromFile(SettingsFilename + ".json");
    static JsonSaveHelper helper(settings, SettingsFilename + ".json");
    return settings;
}
