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

#include "pluginmanager.h"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

void PluginManager::initPluginAPI()
{
    (*boost::dll::import<void(*)(NW_getBlockFunc, NW_setBlockFunc, NW_registerBlockFunc)>
     (PluginApiDllPath, "NW_init", boost::dll::load_mode::append_decorations).get())
    (&World::getBlock, &World::setBlock, &BlockManager::registerBlock);

    setCurrentWorld = boost::dll::import<void(*)(World*)>
                      (PluginApiDllPath, "NW_setCurrentWorld", boost::dll::load_mode::append_decorations);
    setCurrentBlockManager = boost::dll::import<void(*)(BlockManager*)>
                             (PluginApiDllPath, "NW_setCurrentBlockManager", boost::dll::load_mode::append_decorations);
    (*setCurrentBlockManager.get())(&Blocks);
}

void PluginManager::loadPlugins()
{
    using namespace boost::filesystem;
    string path = "./plugins/";
    if (exists(path))
    {
        directory_iterator itemBegin(path), itemEnd;
        for (; itemBegin != itemEnd; itemBegin++)
            if (is_directory(*itemBegin))
            {
                string pluginPath = itemBegin->path().string() + "/" + itemBegin->path().filename().string();
            }
    }
}
