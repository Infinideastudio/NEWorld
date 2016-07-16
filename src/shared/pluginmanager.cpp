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

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "common.h"
#include "utils.h"
#include "pluginmanager.h"
#include "logger.h"

void PluginManager::loadPlugin(const string& filename)
{
    m_plugins.emplace_back(Plugin(filename));
    const Plugin& plugin = m_plugins[m_plugins.size() - 1];
    if (!plugin.isLoaded())
    {
        m_plugins.pop_back();
        warningstream << "Failed to load plugin from \"" << filename << "\", skipping";
        return;
    }
    infostream << "Loaded plugin \"" << plugin.getData().pluginName << "\"["
               << plugin.getData().internalName
               << "], authored by \"" << plugin.getData().authorName << "\"";
}

void PluginManager::loadPlugins()
{
    using namespace boost::filesystem;
    string path = "./../Plugins/";
    if (exists(path))
    {
        directory_iterator itemEnd;
        for (directory_iterator item(path); item != itemEnd; ++item)
            if (!is_directory(*item))
            {
                string pluginPath = item->path().string();
                string suffix = pluginPath.substr(pluginPath.size() - string(DLLSuffix).size());
                strtolower(suffix);
                if (suffix != DLLSuffix) continue;
                loadPlugin(pluginPath);
            }
    }
}
