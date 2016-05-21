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
#include "Audio/AudioSystem.h"
int main()
{
    //Audio Test
    AudioSystem::Init();
    float a1[3] = { 0,0,0 }, a2[3] = { 3,3,3 };
    AudioSystem::Update(a1, true, true, a2, true, false);
    system("pause");
    AudioSystem::UnInit();
    system("pause");
}