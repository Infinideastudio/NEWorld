// 
// GUI: neworld.cpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#include "neworld.h"
#include "gamescene.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
#include "Common/JsonHelper.h"
#include "Game/Context/nwcontext.hpp"
#include "Common/Logger.h"

NEWorld::NEWorld() {
    // Initialize
    getSettings();
    infostream << "Initializing...";
    Window::getInstance("NEWorld", 852, 480);
    Texture::init();
    void registerGUIAPI();
    registerGUIAPI();
}

void NEWorld::run() {
    // Run
    const auto fps = getJsonValue<size_t>(getSettings()["gui"]["fps"], 60);
    const auto shouldLimitFps = getJsonValue<bool>(getSettings()["gui"]["limit"], false);
    const auto delayPerFrame = static_cast<uint32_t>(1000 / fps - 0.5);
    auto& window = Window::getInstance("NEWorld", 852, 480);
    GameScene game("TestWorld", window);
    while (!window.shouldQuit()) {
        // Update
        window.pollEvents();
        // Render
        game.render();
        Renderer::checkError();
        window.swapBuffers();
        if (shouldLimitFps) SDL_Delay(delayPerFrame);
    }
}

NEWorld::~NEWorld() {
    // Terminate
    infostream << "Terminating...";
    Texture::free();
}
