#include <unordered_map>

#include "Typedefs.h"
#include "NsGui/CroppedBitmap.h"
#include "NsGui/Image.h"
#include "NsGui/ImageSource.h"
#include "NsGui/IntegrationAPI.h"
#include "NsGui/TextureSource.h"
#include "NsGui/UIElementData.h"
#include "NsGui/UserControl.h"
#include "Universe/Entity/PlayerEntity.h"

class InventorySlot : public Noesis::UserControl
{
public:
    InventorySlot();

    int getAmount() const noexcept { return mQuantity; }
    void setAmount(int value) { mQuantity = value; SetValue<int>(AmountProperty, value); }
    Item getItem() const noexcept { return mItem; }
    void setItem(Item i) {
        if (mItem == i) return;
	    mItem = i;
        FindName<Noesis::Image>("ItemTexture")->SetSource(getTextureForItem(i));
    }
    void setItemStack(ItemStack stack) {
        setItem(stack.item);
        setAmount(stack.amount);
    } 
    bool isSelected() const noexcept { return mSelected; }
    void setSelected(bool selected) { mSelected = selected; SetValue<bool>(SelectedProperty, selected); }

    static const Noesis::DependencyProperty* AmountProperty;
    static const Noesis::DependencyProperty* SelectedProperty;
    static void clearCache();

private:
    static Noesis::ImageSource* getTextureForItem(Item i);
    static Noesis::Ptr<Noesis::TextureSource> CachedBlockTextures;
    static std::unordered_map<Item, Noesis::Ptr<Noesis::CroppedBitmap>> CachedItemTextures;

    Item mItem;
    bool mSelected;
    int mQuantity;

    NS_IMPLEMENT_INLINE_REFLECTION(InventorySlot, UserControl, "NEWorld.InventorySlot") {
        Noesis::UIElementData* data = NsMeta<Noesis::UIElementData>(Noesis::TypeOf<SelfClass>());
        data->RegisterProperty<int>(AmountProperty, "Amount", Noesis::PropertyMetadata::Create(0));
        data->RegisterProperty<bool>(SelectedProperty, "Selected", Noesis::PropertyMetadata::Create(false));
    }
};
