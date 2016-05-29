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
#ifndef NETWORKSTRUCTURES_H_
#define NETWORKSTRUCTURES_H_
#include <string>
enum Identifier
{
    //Client to server

    //Server to client

    //Common
    Chat
};

class NetworkStructure
{
public:
    virtual void process() = 0;
    virtual ~NetworkStructure() {}
};

class ChatPacket : public NetworkStructure
{
public:
    ChatPacket(std::string userSend, std::string content) :m_userSend(userSend), m_content(content) {}

    virtual void process() override;

private:
    std::string m_userSend;
    std::string m_content;

};

#endif // !NETWORKSTRUCTURES_H_
