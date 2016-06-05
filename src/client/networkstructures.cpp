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

#include "networkstructures.h"
#include <cassert>

#define DEFAULT_IMPLEMENT(classname) void LoginPacket::process() { assert(false); }

//定义了所有数据包的处理方式

void ChatPacket::process()
{
    //输出到屏幕上之类的
}


//以下是服务端部分，客户端无需实现
DEFAULT_IMPLEMENT(LoginPacket)
