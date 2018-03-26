// 
// NEWorld: client.cpp
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
#include "Game/Context/nwcontext.hpp"
#include <iostream>


int main(int argc, char** argv) {
    NEWorld instance{};
    loadModules();
    argagg::parser argparser{
        {
            {
                "help", {"-h", "--help"},
                "shows this help message", 0
            },
            {
                "multiplayer-client", {"-c", "--client"},
                "Start the game as a client of multiplayer session", 0
            }
        }
    };
    try { context.args = argparser.parse(argc, argv); }
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
    instance.run();
    return 0;
}
