#include "Noesis.h"

#include <GL/glew.h>
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
#include "NsGui/BaseValueConverter.h"
#include "NsRender/GLFactory.h"

Noesis::Ptr<Noesis::RenderDevice> GUI::renderDevice;

class Multiplier final : public Noesis::BaseValueConverter {
public:
    bool TryConvert(BaseComponent* value, const Noesis::Type* targetType, BaseComponent* parameter,
                    Noesis::Ptr<BaseComponent>& result) override {
        if (Noesis::Boxing::CanUnbox<double>(value) && Noesis::Boxing::CanUnbox<Noesis::String>(parameter)) {
            double val = Noesis::Boxing::Boxer<double>::Unbox(value);
            Noesis::String mul = Noesis::Boxing::Boxer<Noesis::String>::Unbox(parameter);
            result = Noesis::Boxing::Boxer<double>::Box(val* std::stod(mul.Str()));
            return true;
        }

        return false;
    }

private:
    NS_IMPLEMENT_INLINE_REFLECTION_(Multiplier, BaseValueConverter, "NEWorld.Multiplier")
};

void GUI::noesisSetup() {
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
    if (std::string(NS_LICENSE_NAME).empty()) {
        errorstream << "Noesis License not set.";
        errorstream << "Please add a valid license into External/Noesis/Include/NoesisLicense.h";
        errorstream << "You can get one from https://www.noesisengine.com/trial/";
    }

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
    Noesis::RegisterComponent<Multiplier>();
    Noesis::TypeOf<NoesisApp::Interaction>(); // Force the creation of its reflection type
    Noesis::TypeOf<NoesisApp::StyleInteraction>(); // Force the creation of its reflection type

    // Setup Render Device
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    renderDevice = NoesisApp::GLFactory::CreateDevice(false);
    glPopAttrib();
}


void GUI::noesisShutdown() {
    GUI::renderDevice.Reset();
    Noesis::GUI::Shutdown();
}
