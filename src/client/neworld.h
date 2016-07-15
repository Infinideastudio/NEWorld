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

#ifndef NEWORLD_H_
#define NEWORLD_H_

#include <common.h>
#include "network.h"
#include <thread>
#include <logger.h>
#include "gameview.h"
#include <pluginmanager.h>
#include <world.h>
#include <worldmanager.h>
#include "../server/worldloader.h"

class MainApplication :public UI::Core::Application
{
public:
    MainApplication(int width, int height, const string& title): m_width(width), m_height(height), m_title(title)
    {
    }

    void afterLaunch() override
    {
        addWindow(std::static_pointer_cast<UI::Core::Window>(std::make_shared<MainWindow>(m_width, m_height, m_title)));
    }

private:
    int m_width, m_height;
    string m_title;
};

class NEWorld
{
public:
    NEWorld() :m_world("", m_plugins), m_cpa(8), m_worldLoader(m_world, m_cpa)
    {
        run();
    }

private:
    // Loaded plugins
    PluginManager m_plugins;
    /*
    // Loaded worlds
    WorldManager m_worlds;
    */
    // Current world
    World m_world;
    // CPA
    ChunkPointerArray m_cpa;
    // Loading test
    WorldLoader m_worldLoader;

    // Main procedure for client
    void run();
};

#endif // !NEWORLD_H_
