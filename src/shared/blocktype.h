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

#ifndef BLOCKTYPE_H_
#define BLOCKTYPE_H_

#include <string>

class BlockType
{
private:
    std::string name;
    bool solid;
    bool translucent;
    bool opaque;
    int explodePower;
    int hardness;

public:
    BlockType(const char* name_, bool solid_, bool translucent_, bool opaque_, int explodePower_, int hardness_) :
        name(name_), solid(solid_), translucent(translucent_), opaque(opaque_), explodePower(explodePower_), hardness(hardness_) {}

    // Internal block name
    const std::string& getName() const
    {
        return name;
    }
    // Is solid block
    bool isSolid() const
    {
        return solid;
    }
    // Transparency determines how it will rendered
    bool isTranslucent() const
    {
        return translucent;
    }
    // Opaque means it blocks light
    bool isOpaque() const
    {
        return opaque;
    }
    // Explode power, if it isn't a explosive set this to 0
    int getExplodePower() const
    {
        return explodePower;
    }
    // Hardness
    int getHardness() const
    {
        return hardness;
    }

};

#endif // !BLOCKTYPE_H_
