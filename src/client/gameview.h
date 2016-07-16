/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WINDOW_H_
#define WINDOW_H_

#include <UILib.h>
#include "vec3.h"
#include "renderer.h"

class GameView :public UI::Core::Page
{
public:
    GameView();

    void doRender();
    void onKeyDown(int scancode);
    void onResize(int w, int h);

private:
    int windowWidth = 852, windowHeight = 480;
    Vec3f trans{ 0, -100.0f, 0 };
    Vec3f transSpeed;
    VertexBuffer cube;

};

class MainWindow : public UI::Core::Window
{
public:
    MainWindow(int width, int height, const string& title) : UI::Core::Window(title, width, height, 200, 200)
    {
        // Disable v-sync
        setSwapInterval(0);
        // Load the main menu
        pushPage(std::make_shared<GameView>());
    }
};

#endif
