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

#ifndef RENDERER_H_
#define RENDERER_H_

#include "../shared/vec3.h"
#include "vertexarray.h"

// Renderer理论上是单例？ --qiaozhanrong
class Renderer
{
private:

public:
    // Setup rendering
    void init();
    // Reset translations/rotations (Restore transform matrixes)
    void restoreScale();
    // Apply translations
    void translate(const Vec3<int>& pos);
    // Apply rotations
    void rotate(double degrees, const Vec3<double>& pos);


};

#endif // !RENDERER_H_
