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

#include "settingsmanager.h"
#include "logger.h"
#include "common.h"

void loadSharedSettings(Settings& settings)
{
#ifdef NEWORLD_DEBUG
    clogLevel = settings.get<int>("shared.logger.clogLevel", trace);
    cerrLevel = settings.get<int>("shared.logger.cerrLevel", fatal);
    fileLevel = settings.get<int>("shared.logger.fileLevel", trace);
    lineLevel = settings.get<int>("shared.logger.lineLevel", warning);
#else
    clogLevel = settings.get<int>("shared.logger.clogLevel", info);
    cerrLevel = settings.get<int>("shared.logger.cerrLevel", fatal);
    fileLevel = settings.get<int>("shared.logger.fileLevel", info);
    lineLevel = settings.get<int>("shared.logger.lineLevel", warning);
#endif
    settings.setMinimal(settings.get<bool>("shared.settings.minimal", false));
}
