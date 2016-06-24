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
#include "settings.h"
#include <logger.h>
#include <common.h>
Settings settings("settings.conf");

void loadSettings()
{
#ifdef NEWORLD_DEBUG
    Logger::clogLevel = settings.get<int>("server.logger.clogLevel", Logger::trace);
    Logger::cerrLevel = settings.get<int>("server.logger.cerrLevel", Logger::fatal);
    Logger::fileLevel = settings.get<int>("server.logger.fileLevel", Logger::trace);
    Logger::lineLevel = settings.get<int>("server.logger.lineLevel", Logger::warning);
#else
    Logger::clogLevel = settings.get<int>("server.logger.clogLevel", Logger::info);
    Logger::cerrLevel = settings.get<int>("server.logger.cerrLevel", Logger::fatal);
    Logger::fileLevel = settings.get<int>("server.logger.fileLevel", Logger::info);
    Logger::lineLevel = settings.get<int>("server.logger.lineLevel", Logger::warning);
#endif
    settings.setMinimal(settings.get<bool>("shared.settings.minimal", false));

    saveSettings();
}

void saveSettings()
{
    settings.save();
}
