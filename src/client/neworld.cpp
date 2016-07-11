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

#include "neworld.h"
#include "window.h"
<<<<<<< HEAD
=======
#include "renderer.h"
#include <logger.h>
>>>>>>> renderer

void NEWorld::run()
{
    // Initialize here
<<<<<<< HEAD
    UI::Base::init();
    std::string s = "./Logs";
    UI::Logger::init(s.c_str());
    //UI::Font::service.addSearchPaths({ "C:/Windows/Fonts" , "." });
    //UI::Globalization::Service::getInstance().attachLangFiles({ "chinese", "english" });

    loggerInit("NEWorld");
    std::thread serverThread(networkThread);
    App app(852, 480, "NEWorld");

    // Start to run
    app.run();

    // Destroy here
    UI::Logger::service.dump();
    serverThread.join();
=======
    loggerInit("NEWorld");
    infostream << "Initializing...";
    bool exit = false;
    //std::thread serverThread(networkThread);
    Window mainWindow(852, 480, "NEWorld");
    Renderer::init(852, 480);

    // Start to run
    infostream << "Game start!";
    float xrot = 0.0f, yrot = 0.0f;
    while (!exit)
    {
        Renderer::clear();
        Renderer::restoreProj();
        Renderer::applyPerspective(60.0f, 852.0f / 480.0f, 0.1f, 100.0f);
        Renderer::restoreScale();
        Renderer::translate(Vec3f(0.0f, 0.0f, -3.0f));
        Renderer::rotate(xrot, Vec3f(1.0f, 0.0f, 0.0f));
        Renderer::rotate(yrot, Vec3f(0.0f, 1.0f, 0.0f));

        glBegin(GL_QUADS);
        // Front
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f,-0.5f, 0.5f);
        glVertex3f( 0.5f,-0.5f, 0.5f);
        glVertex3f( 0.5f, 0.5f, 0.5f);
        // Back
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f( 0.5f, 0.5f,-0.5f);
        glVertex3f( 0.5f,-0.5f,-0.5f);
        glVertex3f(-0.5f,-0.5f,-0.5f);
        glVertex3f(-0.5f, 0.5f,-0.5f);
        // Top
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f(-0.5f, 0.5f,-0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f( 0.5f, 0.5f, 0.5f);
        glVertex3f( 0.5f, 0.5f,-0.5f);
        // Bottom
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(-0.5f,-0.5f, 0.5f);
        glVertex3f(-0.5f,-0.5f,-0.5f);
        glVertex3f( 0.5f,-0.5f,-0.5f);
        glVertex3f( 0.5f,-0.5f, 0.5f);
        // Right
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f( 0.5f,-0.5f, 0.5f);
        glVertex3f( 0.5f,-0.5f,-0.5f);
        glVertex3f( 0.5f, 0.5f,-0.5f);
        glVertex3f( 0.5f, 0.5f, 0.5f);
        // Left
        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex3f(-0.5f,-0.5f,-0.5f);
        glVertex3f(-0.5f,-0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f,-0.5f);
        glEnd();

        glFlush();
        glFinish();

        mainWindow.swapBuffers();
        Window::update();
        if (mainWindow.isKeyPressed(GLFW_KEY_LEFT)) yrot -= 0.25f;
        if (mainWindow.isKeyPressed(GLFW_KEY_RIGHT)) yrot += 0.25f;
        if (mainWindow.isKeyPressed(GLFW_KEY_UP)) xrot -= 0.25f;
        if (mainWindow.isKeyPressed(GLFW_KEY_DOWN)) xrot += 0.25f;
        if (mainWindow.closing()) exit = true;
    }

    // Destroy here
    infostream << "Terminating...";
    //serverThread.join();
>>>>>>> renderer
    disconnect();
}
