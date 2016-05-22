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

#ifndef NETWORK_H_
#define NETWORK_H_

#include <boost/asio.hpp>
#include "network_structures.h"
#include "request.h"

using boost::asio::ip::tcp;

constexpr int PacketMaxLength = 256 * 1024; //256kb
constexpr int Port = 8090;
extern boost::asio::io_service io_service;

#endif // RECEIVER_H_
