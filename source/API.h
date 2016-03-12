#pragma once
#include <functional>
#include <string>
#include <map>
#include <vector>
#include "Definitions.h"
#include "World.h"
#include "Chunk.h"
#include "Command.h"
#include "ModSupport.h"
extern vector<Command> commands;

namespace Mod
{

extern std::map<std::string, void*> sharedData;

APIPackage getPackage();

}
