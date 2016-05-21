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

#ifndef REQUEST_H_
#define REQUEST_H_

#include <functional>

// Used for the requests that have callback
class RequestCallback
{
public:
    /*
    callbackFunc() is a callback function.
    Its first parameter gives the pointer to data, and the second one give the length of the data.
    */
    using callbackFunc = std::function<void(const char*, size_t)>;
    struct RequestID
    {
        unsigned short serverID;
        unsigned short clientID;
    };
    RequestID identifier;
    callbackFunc callback;

    friend RequestCallback makeRequestCallback(const RequestCallback::callbackFunc& func);

private:
    RequestCallback(const callbackFunc& func, RequestID id) :callback(func), identifier(id) {}

};

RequestCallback makeRequestCallback(const RequestCallback::callbackFunc& func);

#endif // !REQUEST_H_
