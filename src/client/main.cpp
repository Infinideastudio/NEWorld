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

#include "neworld.h"
#include <logger.h>
#undef main
extern "C" NWAPIEXPORT int NWAPICALL main(int, char*[]);

// Just a joke :D
class orld
{
public:
    orld()
    {
        NEWorld neworld;
    }
};

int NWAPICALL main(int, char*[])
{
    // NOW THE WORLD CONSTRUCTS!
    new orld;
    // NOW EVERYTHING COMES TO THE END...
    return 0;
}
