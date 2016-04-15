#include "Items.h"
#include "Textures.h"

ItemInfo itemsinfo[] = { STICK, APPLE };

void LoadItemsTextures()
{
    itemsinfo[BuiltInItems::STICK - theFirstItem.ID].texture =
        Textures::LoadRGBTexture("res/Textures/Items/stick.bmp");
    itemsinfo[BuiltInItems::APPLE - theFirstItem.ID].texture =
        Textures::LoadRGBTexture("res/Textures/Items/apple.bmp");

}
