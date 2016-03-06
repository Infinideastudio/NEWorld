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


#ifndef _NETYCAT_BUFFERCONDiTION_H_
#define _NETYCAT_BUFFERCONDiTION_H_


#include <cstdint>

#include "Buffer.h"


namespace Netycat {
    
    namespace Core {
        
        class BufferCondition {
            
            public:
            
            BufferCondition();
            BufferCondition(const BufferCondition& src);
            virtual ~BufferCondition() = 0;
            
            virtual uintptr_t control(Buffer& buffer, uintptr_t recvBufferSize) = 0;
            
        };
        
        class BufferConditionExactLength : public BufferCondition {
            
            private:
            
            uintptr_t length;
            
            public:
            
            BufferConditionExactLength(uintptr_t len);
            BufferConditionExactLength(const BufferConditionExactLength& src);
            ~BufferConditionExactLength();
            
            uintptr_t control(Buffer& buffer, uintptr_t recvBufferSize);
            
        };
        
        class BufferConditionLeastLength : public BufferCondition {
            
            private:
            
            uintptr_t length;
            
            public:
            
            BufferConditionLeastLength(uintptr_t len);
            BufferConditionLeastLength(const BufferConditionLeastLength& src);
            ~BufferConditionLeastLength();
            
            uintptr_t control(Buffer& buffer, uintptr_t recvBufferSize);
            
        };
        
        class BufferConditionMeetkeyword : public BufferCondition {
            
            private:
            
            uint8_t* keyword;
            uintptr_t length;
            
            public:
            
            BufferConditionMeetkeyword(uint8_t* key, uintptr_t len);
            BufferConditionMeetkeyword(const BufferConditionMeetkeyword& src);
            ~BufferConditionMeetkeyword();
            
            uintptr_t control(Buffer& buffer, uintptr_t recvBufferSize);
            
        };
        
    }
    
}


#endif

