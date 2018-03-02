// 
// GUI: mainclient.cpp
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

#include "neworld.h"
#include <argagg.hpp>
#include "game/context/nwcontext.hpp"
#include <iostream>
#include "Common/EventBus.h"

namespace
{    
int guiMain(int argc, char** argv) {
    argagg::parser argparser{ {
        { "help",{ "-h", "--help" },
        "shows this help message", 0 },
        { "multiplayer-client",{ "-c", "--client" },
        "Start the game as a client of multiplayer session", 0 }
        } };
    try {
        context.args = argparser.parse(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    if (context.args["help"]) {
        argagg::fmt_ostream fmt(std::cerr);
        fmt << "Usage:" << std::endl
            << argparser;
        return 0;
    }
    NEWorld neworld;
    return 0;
}

}

extern "C" {

    NWAPIEXPORT NWplugindata* NWAPICALL getInfo() {
        auto plugin = new NWplugindata();
        plugin->pluginName = "NEWorld";
        plugin->authorName = "INFINIDEAS";
        plugin->internalName = "infinideas.neworld.gui";
        plugin->pluginType = nwPluginTypeGUI;
        return plugin;
    }

    // Main function
    NWAPIEXPORT void NWAPICALL init(NWplugintype type) {
        REGISTER_AUTO(guiMain);
    }

    // Unload function
    NWAPIEXPORT void NWAPICALL unload() { }
}

constexpr const char* info = R"(
{
    "name" = "GUI",
    "author" = "INFINIDEAS",
    "uri" = "infinideas.neworld.gui",
    "version" = "0.0.0.1",
    "conflicts" = "0.0.0.0"
}
)";