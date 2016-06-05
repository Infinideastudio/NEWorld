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
#include <cstdint>
enum Identifier :uint32_t
{
    //Client to server (0 ~ 2^30-1)
    Login = 0,
    //Server to client (2^30 ~ 2*2^30-1)
    Placeholder = 1 << 30,
    //Common (2*2^30 ~ 3^30-1)
    Chat = (1 << 30)*2u
};

class NetworkStructure
{
public:
    virtual void process() = 0;
    virtual ~NetworkStructure() {}
};
class LoginPacket : public NetworkStructure
{
public:
    LoginPacket(std::string username, std::string password, uint16_t version) :
        m_username(username), m_password(password), m_version(version) {}

    virtual void process() override;

    Packet makePacket()
    {
        Packet p;
        p.identifier = Identifier::Login;

    }

private:
    std::string m_username;
    std::string m_password;
    uint16_t m_version;

};
class ChatPacket : public NetworkStructure
{
public:
    ChatPacket(std::string userSend, std::string content) :
        m_userSend(userSend), m_content(content) {}

    virtual void process() override;

private:
    std::string m_userSend;
    std::string m_content;

};

#endif // !NETWORKSTRUCTURES_H_
