#pragma once

#include "Definitions.h"

class ItemInfo {
public:
    ItemInfo(Item itemid, TextureID itemtexture = 0) : id(itemid), texture(itemtexture) {}

    Item id;
    TextureID texture;
};

enum BuiltInItems {
    STICK = 30000, APPLE
};

extern ItemInfo itemsinfo[];
const Item theFirstItem = STICK;

void loadItemsTextures();

inline bool isBlock(Item i) {
    return i < theFirstItem;
}

inline TextureID getItemTexture(Item i) {
    if (isBlock(i)) return BlockTextures;
    else return itemsinfo[i - theFirstItem].texture;
}