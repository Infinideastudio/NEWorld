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

#include "pluginsdk.h"
#include "defs.h"
#include "export_variables.h"
#include <fstream>
#include <string>
#include <map>

std::map<std::wstring, internal_plugin_info> plugins;
std::map<std::wstring, void*> shared_data;

void load_plugins(wchar_t const * const filename)
{
#ifdef _WIN32
    std::wifstream file(filename, std::ios::in);
#elif NEWORLD_TARGET_MACOSX
    std::wifstream file;
    std::unique_ptr<char> tmp(new char[(wcslen(filename)+1)*3]);
    wcstombs(tmp.get(), filename, (wcslen(filename)+1));
    file.open(tmp.get());
#endif
    std::wstring plugin_location;
    while (file >>  plugin_location)
    {
        dll_handle handle = load_library(plugin_location.c_str());
        get_information get_info_proc = (get_information)get_address(handle, "get_information");
        plugin_information* info = get_info_proc();
        std::wstring plugin_name = info->name, plugin_desc = info->description;
        init_function init = (init_function)get_address(handle,"on_init");
        if (init != nullptr && !init())
            unload_library(handle);
        continue;
        plugins[plugin_name] = internal_plugin_info(handle, plugin_name, plugin_desc);
        
    }
}

void unload_plugins()
{
    for (std::map<std::wstring, internal_plugin_info>::iterator it = plugins.begin(); it != plugins.end(); it++)
    {
        dll_handle handle = it->second.handle;
        unload_function unload = (unload_function)get_address(handle, "on_unload");
        if (unload != nullptr)
            unload();
        unload_library(handle);
    }
    plugins.clear();
}

void * get_shared_data(wchar_t const * const key)
{
    return shared_data[key];
}

void set_shared_data(wchar_t const * const key, void* const data)
{
    shared_data[key] = data;
}

command_function get_command(wchar_t const * const name)
{
    return commands[name].function;
}

void register_command(wchar_t const * const name, command_function func)
{
    commands[name] = command(name, func);
}

unsigned short get_block(int x, int y, int z)
{
    return get_func(x, y, z);
}

void update_block(int x, int y, int z, char val)
{
    update_func(x, y, z, val != 0);
}
