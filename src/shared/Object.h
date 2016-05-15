/*
* NEWorld: An free game with similar rules to Minecraft.
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

#ifndef OBJECT_H_
#define OBJECT_H_

#include "aabb.h"
#include "vec3.h"

class Object
{
private:
    DoubleVec3 _position, _size, _rotation;
    int _health;
    AABB _hitbox;

public:
    int getHealth() const { return _health; }
    void setHealth(int val) { _health = val; }

    DoubleVec3 getPosition() const { return _position; }
    void setPosition(DoubleVec3 val) { _position = val; }

    DoubleVec3 getRotation() const { return _rotation; }
    void setRotation(DoubleVec3 val) { _rotation = val; }

    DoubleVec3 getSize() const { return _size; }

    AABB getHitbox() const { return _hitbox; }

    virtual void update() = 0;

};

#endif // OBJECT_H_
