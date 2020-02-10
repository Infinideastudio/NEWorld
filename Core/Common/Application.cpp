//
// Core: Application.cpp
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

#include <iostream>
#include <Cfx/Application.h>

#include "Application.h"
#include "Logger.h"

namespace {
    Application* appInstanceHandle = nullptr;
    argagg::parser parser;
    argagg::parser_results pargs;
}

Application::Application() {
    if (!appInstanceHandle)
        appInstanceHandle = this;
    else {
        errorstream << "Double Application Instance! Stopping";
        throw std::runtime_error("Double App Instance");
    }
}

void Application::run() {
	errorstream << "No application is specified.";
}

argagg::parser_results& Application::args() { return pargs; }

CmdOption::CmdOption(argagg::definition def) {
    parser.definitions.push_back(std::move(def));
}

NWAPIEXPORT int gameMain(int argc, char** argv) {
    try { pargs = parser.parse(argc, argv); }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    if (pargs["help"]) {
        argagg::fmt_ostream fmt(std::cerr);
        fmt << "Usage:" << std::endl << parser;
        return 0;
    }
    if (appInstanceHandle)
        appInstanceHandle->run();
    return 0;
}
