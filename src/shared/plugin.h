/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PLUGIN_H_
#define PLUGIN_H_

#include <string>
#include <cstring> // memcpy
using std::string;
#include "vec3.h"
#include "blocktype.h"
#include "blockdata.h"

class PluginData
{
public:
    // Declared block types
    int blocksCount;
    BlockType* blocks = nullptr;
    // Declared terrain generator
    BlockData* (*buildChunk)(const Vec3i&) = nullptr;
};

class Plugin
{
private:
    string name;
    boost::shared_ptr<void(*)()> init;
    PluginData data;

public:
    Plugin()
    {}
    explicit Plugin(const string& filename)
    { loadFrom(filename); }
    Plugin(const Plugin&) = delete;

    // Get plugin name
    const string& getName() const
    { return name; }
    // Get plugin data
    const PluginData& getData() const
    { return data; }
    // Load plugin, return 0 for success
    int loadFrom(const string& filename);

};

#endif // !PLUGIN_H_
