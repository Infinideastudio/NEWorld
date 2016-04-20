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
