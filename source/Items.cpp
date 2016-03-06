#include "Items.h"
#include "Textures.h"

ItemInfo itemsinfo[] = { STICK, APPLE };

void loadItemsTextures()
{
    itemsinfo[BuiltInItems::STICK - theFirstItem].texture =
        Textures::LoadRGBTexture("Textures/Items/stick.bmp");
    itemsinfo[BuiltInItems::APPLE - theFirstItem].texture =
        Textures::LoadRGBTexture("Textures/Items/apple.bmp");

}
