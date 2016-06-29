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
#include <vector>
#include <boost/dll/shared_library.hpp>
#include "plugin.h"

// Plugin system
class PluginManager
{
private:
    std::vector<Plugin> m_plugins;

public:
    // Load single plugin
    void loadPlugin(const string& filename);
    // Load plugins
    void loadPlugins();
};

#endif // !PLUGINMANAGER_H_
