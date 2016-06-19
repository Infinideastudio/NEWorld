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

#ifndef PLUGINMANAGER_H_
#define PLUGINMANAGER_H_

#include <string>
using std::string;
#include <boost/dll/shared_library.hpp>
#include "plugin.h"
#include "../pluginapi/pluginapi.h"

// For API declarations
class World;
class BlockManager;

const string PluginApiDllPath = "PluginAPI";

// Plugin system
class PluginManager
{
public:
    PluginManager() :pluginAPI(PluginApiDllPath, boost::dll::load_mode::append_decorations)
    {}

    // Initialize PluginAPI DLL
    void initPluginAPI();
    // Load plugins
    void loadPlugins();

private:
    // PluginAPI DLL library
    boost::dll::shared_library pluginAPI;
    // Set current World
    boost::shared_ptr<void(*)(World*)> setCurrentWorld;
    // Set current BlockManager
    boost::shared_ptr<void(*)(BlockManager*)> setCurrentBlockManager;

};

#endif // !PLUGINMANAGER_H_
