#include "Noesis.h"

#include <NsApp/StyleInteraction.h>
#include <NsApp/EventTrigger.h>
#include <NsApp/TriggerCollection.h>
#include <NsCore/RegisterComponent.inl>
#include <NsApp/GoToStateAction.h>
#include <NsApp/KeyTrigger.h>
#include <NsApp/MouseDragElementBehavior.h>
#include <NsApp/ComparisonCondition.h>
#include <NsCore/EnumConverter.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/ThemeProviders.h>
#include <NsApp/LocalTextureProvider.h>
#include <NsApp/LocalFontProvider.h>
#include <NsApp/LocalXamlProvider.h>
#include <NsApp/BehaviorCollection.h>
#include <NsApp/Interaction.h>
#include "Common/Logger.h"

void GUI::noesisSetup()
{
    Noesis::SetLogHandler([](const char*, uint32_t, uint32_t level, const char*, const char* msg) {
        Logger::Level prefixes[] = {
            Logger::Level::debug,
            Logger::Level::debug,
            Logger::Level::info,
            Logger::Level::warning,
            Logger::Level::error
        };
        Logger(__FILE__, __FUNCTION__, __LINE__, prefixes[level], "Noesis") << msg;
        });

    // Sets the active license
    Noesis::GUI::SetLicense(NS_LICENSE_NAME, NS_LICENSE_KEY);

    // Noesis initialization. This must be the first step before using any NoesisGUI functionality
    Noesis::GUI::Init();

    Noesis::Ptr<Noesis::XamlProvider> xamlProvider =
        *new NoesisApp::LocalXamlProvider("Assets/GUI");
    Noesis::Ptr<Noesis::FontProvider> fontProvider =
        *new NoesisApp::LocalFontProvider("Assets/Fonts");
    Noesis::Ptr<Noesis::TextureProvider> textureProvider =
        *new NoesisApp::LocalTextureProvider("Assets/Textures");
    NoesisApp::SetThemeProviders(xamlProvider, fontProvider, textureProvider);

    Noesis::GUI::LoadApplicationResources("Theme/NEWorld.xaml");
    Noesis::RegisterComponent<NoesisApp::BehaviorCollection>();
    Noesis::RegisterComponent<NoesisApp::TriggerCollection>();
    Noesis::RegisterComponent<NoesisApp::EventTrigger>();
    Noesis::RegisterComponent<NoesisApp::DataTrigger>();
    Noesis::RegisterComponent<NoesisApp::MouseDragElementBehavior>();
    Noesis::RegisterComponent<NoesisApp::GoToStateAction>();
    Noesis::TypeOf<NoesisApp::Interaction>(); // Force the creation of its reflection type
    Noesis::TypeOf<NoesisApp::StyleInteraction>(); // Force the creation of its reflection type
}
