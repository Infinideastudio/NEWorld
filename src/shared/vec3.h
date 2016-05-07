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

#ifndef VEC3_H_
#define VEC3_H_

class Vec3
{
public:
    int x, y, z;

    Vec3::Vec3() :x(0), y(0), z(0) {}
    Vec3::Vec3(int x_, int y_, int z_) :x(x_), y(y_), z(z_) {}
    Vec3::Vec3(const Vec3& rhs) :x(rhs.x), y(rhs.y), z(rhs.z) {}

    void operator= (const Vec3& rhs)
    {
        x = rhs.x; y = rhs.y; z = rhs.z;
    }

    bool operator< (const Vec3& rhs) const
    {
        if (x < rhs.x) return true; if (x > rhs.x) return false;
        if (y < rhs.y) return true; if (y > rhs.y) return false;
        if (z < rhs.z) return true; return false;
    }

    bool operator== (const Vec3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    bool operator!= (const Vec3& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }
    Vec3 operator+ (const Vec3& rhs) const { return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); }
    Vec3 operator- (const Vec3& rhs) const { return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); }

};

#endif // !VEC3_H_
