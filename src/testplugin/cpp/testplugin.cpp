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

#include "../../../api/cpp/nwapi.h"

NWplugindata* TestPlugin = nullptr;

// Export functions
extern "C"
{
    NWAPIEXPORT NWplugindata* NWAPICALL init();
    NWAPIEXPORT void NWAPICALL unload();
}

// Main function
NWplugindata* NWAPICALL init()
{
    NWblocktype rock;
    rock.blockname = "Rock";
    rock.solid = 1;
    rock.translucent = 0;
    rock.opaque = 1;
    rock.explodePower = 0;
    rock.hardness = 2;
    nwRegisterBlock(&rock);
    TestPlugin = new NWplugindata();
    TestPlugin->pluginName = "Test Plugin";
    TestPlugin->authorName = "INFINIDEAS";
    TestPlugin->internalName = "infinideas.testplugin_cpp";
    return TestPlugin;
}

void NWAPICALL unload()
{
    if (TestPlugin != nullptr) delete TestPlugin;
}
