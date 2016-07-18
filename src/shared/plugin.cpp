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

#include "plugin.h"
#include "logger.h"

typedef PluginData* NWAPICALL InitFunction();
typedef void NWAPICALL UnloadFunction();

int Plugin::loadFrom(const string& filename)
{
    InitFunction* init = nullptr;
    try
    {
        m_lib.load(filename);
        init = m_lib.get<InitFunction>("init");
        m_data = init();
    }
    catch (std::exception& e)
    {
        if (!m_lib.is_loaded()) return m_status = 1; // Failed: could not load
        if (init == nullptr) return m_status = 2; // Failed: entry not found
        warningstream << "Failed: unhandled exception: " << e.what();
    }
    return m_status = 0;
}

void Plugin::unload()
{
    if (m_status != 0) return;
    m_status = -1;
    UnloadFunction* unload = nullptr;
    try
    {
        unload = m_lib.get<UnloadFunction>("unload");
        unload();
        m_lib.unload();
    }
    catch (std::exception& e)
    {
        if (unload == nullptr)
        {
            // Warning: entry not found
            warningstream << "Subroutine unload() not found in plugin " << m_data->internalName << ", skipped unloading but may cause memory leak";
            return;
        }
        warningstream << "Failed: unhandled exception: " << e.what();
    }
}
