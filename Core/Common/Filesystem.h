// 
// Core: Filesystem.h
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

#pragma once

#include "Config.h"

#include <filesystem>
namespace filesystem = std::filesystem;

NWCOREAPI void fsInit(const char* argv0);
NWCOREAPI filesystem::path executablePath();
NWCOREAPI filesystem::path assetDir(const char* moduleName);
NWCOREAPI filesystem::path assetDir(const std::string& moduleName);
NWCOREAPI filesystem::path dataDir(const char* moduleName);
NWCOREAPI filesystem::path dataDir(const std::string& moduleName);
