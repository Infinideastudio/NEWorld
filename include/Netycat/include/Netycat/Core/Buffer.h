/*******************************************************************************
    
    Copyright 2015 SuperSodaSea
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
    
********************************************************************************
    
*******************************************************************************/


#ifndef _NETYCAT_BUFFER_H_
#define _NETYCAT_BUFFER_H_


#include <cstdint>


namespace Netycat {
    
    namespace Core {
        
        class Buffer{
            
            private:
            
            uint8_t* data;
            uintptr_t readpos;
            uintptr_t writepos;
            uintptr_t remaining;
            uintptr_t capacity;
            
            public:
            
            Buffer(uintptr_t len);
            Buffer(const Buffer& src);
            ~Buffer();
            
            bool read(void* dat, uintptr_t len);
            bool readU16(uint16_t* dat, uintptr_t len);
            bool readU32(uint32_t* dat, uintptr_t len);
            bool readBuffer(Buffer& dst, uintptr_t len);
            
            bool write(const void* dat, uintptr_t len);
            bool writeU16(const uint16_t* dat, uintptr_t len);
            bool writeU32(const uint32_t* dat, uintptr_t len);
            bool writeBuffer(Buffer& src, uintptr_t len);
            
            bool skip(uintptr_t len);
            
            uint8_t at(uintptr_t index);
            
            bool reset();
            
            uintptr_t getCapacity();
            bool setCapacity(uintptr_t cap);
            
            uintptr_t getRemaining();
            
			uint8_t* getData() { return data; }
        };
        
    }
    
}


#endif

