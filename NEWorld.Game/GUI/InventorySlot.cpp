#include "InventorySlot.h"

#include <unordered_map>

#include "Definitions.h"
#include "Textures.h"
#include "Typedefs.h"
#include "Universe/World/Blocks.h"

#include <NsDrawing/Int32Rect.h>
#include <NsGui/CroppedBitmap.h>
#include <NsGui/TextureSource.h>
#include <NsRender/GLFactory.h>
#include <NsRender/Texture.h>

const Noesis::DependencyProperty* InventorySlot::AmountProperty;
const Noesis::DependencyProperty* InventorySlot::SelectedProperty;

Noesis::Ptr<Noesis::TextureSource> InventorySlot::CachedBlockTextures;
std::unordered_map<item, Noesis::Ptr<Noesis::CroppedBitmap>> InventorySlot::CachedItemTextures;

InventorySlot::InventorySlot() {
    Noesis::GUI::LoadComponent(this, "InventorySlot.xaml");
    if (!CachedBlockTextures) {
        CachedBlockTextures = Noesis::MakePtr<Noesis::TextureSource>(NoesisApp::GLFactory::WrapTexture(
            BlockTextures, 256, 256, 0, false, true
        ));
    }
}

void InventorySlot::clearCache() {
    CachedItemTextures.clear();
}

Noesis::ImageSource* InventorySlot::getTextureForItem(item i) {
    if (i == Blocks::ENV) return nullptr;
    // find from cache first
    auto itemTextureIter = CachedItemTextures.find(i);

    if (itemTextureIter != CachedItemTextures.end())
        return (*itemTextureIter).second.GetPtr();

    const auto tcX = Textures::getTexcoordX(i, 1) * 256;
    const auto tcY = Textures::getTexcoordY(i, 1) * 256;
    return CachedItemTextures[i] = Noesis::MakePtr<Noesis::CroppedBitmap>(
        CachedBlockTextures.GetPtr(), Noesis::Int32Rect(tcX, tcY, 32, 32)  // TODO: refactor
        );
}
