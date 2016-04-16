#include "Items.h"
#include "Textures.h"

ItemInfo itemsinfo[] = { item(STICK), item(APPLE) };

void LoadItemsTextures()
{
    itemsinfo[BuiltInItems::STICK - theFirstItem.ID].texture =
        Textures::LoadRGBTexture("Textures/Items/stick.bmp");
    itemsinfo[BuiltInItems::APPLE - theFirstItem.ID].texture =
        Textures::LoadRGBTexture("Textures/Items/apple.bmp");

}
