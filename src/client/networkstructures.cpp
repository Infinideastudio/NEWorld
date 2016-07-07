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

#include "networkstructures.h"
#include <cassert>

#define DEFAULT_IMPLEMENT(classname) void classname::process() { assert(false); }

//Define the ways the data packets process.

void ChatPacket::process()
{
    //Do something like printing it to the screen.
}

//The below is server side, no need for client to implement.
DEFAULT_IMPLEMENT(LoginPacket)
