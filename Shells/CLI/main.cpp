// 
// CLI: Main.cpp
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

#include "cli.hpp"
#include "Common/JsonHelper.h"
#include "Game/Server/server.h"

int NWAPIEXPORT main(int argc, char* argv[]) {
    ServerCommandLine cli;
    Server server(getJsonValue<unsigned short>(getSettings()["server"]["port"], 31111));
    server.run(getJsonValue<size_t>(getSettings()["server"]["rpc_thread_number"], 1));
    cli.start();
}
