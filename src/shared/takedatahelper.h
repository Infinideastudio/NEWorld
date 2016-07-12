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

#ifndef TAKEDATAHELPER_H_
#define TAKEDATAHELPER_H_
#include <memory>

/// A helper class for taking data out of a byte array
class TakeDataHelper
{
public:
    TakeDataHelper(std::shared_ptr<char> buffer, size_t length)
        : m_shared_ptr(buffer), m_length(length), m_buffer(buffer.get()), m_offset(0)
    {
    }

    template <typename T>
    T take()
    {
        if (m_offset + sizeof(T) > m_length) throw;
        T ret = *reinterpret_cast<T*>(m_buffer + m_offset);
        m_offset += sizeof(T);
        return ret;
    }

    std::string getString(size_t length)
    {
        if (m_offset + length >= m_length) throw;
        char* ret = m_buffer + m_offset;
        m_offset += length;
        return std::string(ret, length);
    }

private:
    char* m_buffer;
    size_t m_length;
    size_t m_offset;
    std::shared_ptr<char> m_shared_ptr;
};

#endif // !TAKEDATAHELPER_H_
