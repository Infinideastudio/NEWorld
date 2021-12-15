#pragma once
#include "NsCore/Ptr.h"
#include "NsRender/RenderDevice.h"

namespace GUI {
    void noesisSetup();
    void noesisShutdown();

	extern Noesis::Ptr<Noesis::RenderDevice> renderDevice;
}
