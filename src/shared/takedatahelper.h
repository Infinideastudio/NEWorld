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

#include <boost/core/noncopyable.hpp>
#include <string>

/// A helper class for taking data out of a byte array
class TakeDataHelper :boost::noncopyable
{
public:
    TakeDataHelper(char* buffer, size_t length, bool autoReleaseArray)
        :m_buffer(buffer), m_length(length), m_offset(0), m_autoReleaseArray(autoReleaseArray) {}
    ~TakeDataHelper()
    {
        if(m_autoReleaseArray) delete[] m_buffer;
    }

    template<typename T>
    T take()
    {
        if (m_offset + sizeof(T) > m_length) throw;
        T ret = *((T*)(m_buffer + m_offset));
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
    bool m_autoReleaseArray;

};

#endif // !TAKEDATAHELPER_H_
