// 
// GUI: gamescene.h
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

#pragma once

#include "Common/RateController.h"
#include "client/player.h"
#include "widgetmanager.h"
#include "window.h"
#include "renderer/texture.h"
#include "renderer/worldrenderer.h"
#include "server/server.h"

class GameScene {
public:
    // GameScene update frequency
    static constexpr int UpdateFrequency = 30;

    GameScene(const std::string& name, const Window& window);
    ~GameScene();

    void render();
private:
    size_t requestWorld();

    void keyboardUpdateTask();

    // Local server
    std::unique_ptr<Server> mServer = nullptr;
    // Window
    const Window& mWindow;
    // Texture test
    Texture mTexture;
    // Player
    Player mPlayer;
    // Widget manager
    WidgetManager mGUIWidgets;
    // Update scheduler
    RateController mUpdateScheduler{UpdateFrequency};
    // Rate counters
    size_t mFpsCounter, mUpsCounter, mFpsLatest, mUpsLatest;
    // Current world
    World* mCurrentWorld;
    // World renderer
    std::unique_ptr<WorldRenderer> mWorldRenderer;

    RateController mRateCounterScheduler{1};
};
