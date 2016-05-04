/*
 * NEWorld: An free game with similar rules to Minecraft.
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

#ifndef ITEMS_H
#define ITEMS_H

#include "Definitions.h"

class ItemInfo
{
    public:
        ItemInfo(item itemid, TextureID itemtexture=0) :id(itemid), texture(itemtexture) {}
        item id;
        TextureID texture;
};

enum BuiltInItems
{
    STICK = 30000, APPLE
};

extern ItemInfo itemsinfo[];
const item theFirstItem = item(STICK);

void LoadItemsTextures();

inline bool isBlock(item i)
{
    return i.ID < theFirstItem.ID;
}

inline TextureID getItemTexture(item i)
{
    return isBlock(i) ? BlockTextures : itemsinfo[i.ID - theFirstItem.ID].texture;
}

#endif
