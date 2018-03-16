// 
// GUI: renderer.cpp
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

#include "renderer.h"

int Renderer::matrixMode = 0;

void Renderer::init() {
    debugstream << "Start to initialize GLEW...";
    const auto err = glewInit();
    if (err != GLEW_OK)
        fatalstream << "Failed to initialize GLEW! Error code: "
            << err << ": " << glewGetErrorString(err);
    else
        debugstream << "GLEW initialized!";
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DITHER);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}
