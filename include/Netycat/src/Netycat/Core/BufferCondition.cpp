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


#include <cstdint>
#include <cstdio>

#include "..\..\..\include\Netycat\Core\Buffer.h"
#include "..\..\..\include\Netycat\Core\BufferCondition.h"


namespace Netycat {
    
    namespace Core {
        
        BufferCondition::BufferCondition() {
            
        }
        
        BufferCondition::BufferCondition(const BufferCondition&) {
            
        }
        
        BufferCondition::~BufferCondition() {
            
        }
        
        
        BufferConditionExactLength::BufferConditionExactLength(uintptr_t len) :
            length(len) {
            
        }
        
        BufferConditionExactLength::BufferConditionExactLength(const BufferConditionExactLength& src) :
            length(src.length) {
            
        }
        
        BufferConditionExactLength::~BufferConditionExactLength() {
            
        }
        
        uintptr_t BufferConditionExactLength::control(Buffer& buffer, uintptr_t recvBufferSize) {
            
            uintptr_t remaining = buffer.getRemaining();
            
            if(remaining < length) {
                
                return remaining + recvBufferSize;
                
            } else {
                
                return length;
                
            }
            
        }
        
        
        BufferConditionLeastLength::BufferConditionLeastLength(uintptr_t len) :
            length(len) {
            
        }
        
        BufferConditionLeastLength::BufferConditionLeastLength(const BufferConditionLeastLength& src) :
            length(src.length) {
            
        }
        
        BufferConditionLeastLength::~BufferConditionLeastLength() {
            
        }
        
        uintptr_t BufferConditionLeastLength::control(Buffer& buffer, uintptr_t recvBufferSize) {
            
            uintptr_t remaining = buffer.getRemaining();
            
            if(remaining < length) {
                
                return remaining + recvBufferSize;
                
            } else {
                
                return remaining;
                
            }
            
        }
        
        
        BufferConditionMeetkeyword::BufferConditionMeetkeyword(uint8_t* key, uintptr_t len) :
            keyword(new uint8_t[len]),
            length(len) {
            
            for(uintptr_t i = 0; i < len; i++) {
                
                keyword[i] = key[i];
                
            }
            
        }
        
        BufferConditionMeetkeyword::BufferConditionMeetkeyword(const BufferConditionMeetkeyword& src) :
            keyword(new uint8_t[src.length]),
            length(src.length) {
            
            for(uintptr_t i = 0; i < length; i++) {
                
                keyword[i] = src.keyword[i];
                
            }
            
        }
        
        BufferConditionMeetkeyword::~BufferConditionMeetkeyword() {
            
        }
        
        uintptr_t BufferConditionMeetkeyword::control(Buffer& buffer, uintptr_t recvBufferSize) {
            
            uintptr_t remaining = buffer.getRemaining();
            bool found = false;
            uintptr_t pos = 0;
            
            while(!found && (pos + length) <= remaining) {
                
                found = true;
                for(uintptr_t i = 0; i < length; i++) {
                    
                    if(keyword[i] != buffer.at(pos + i)) {
                        
                        found = false;
                        break;
                        
                    }
                    
                }
                
                pos++;
                
            }
            
            if(found) {
                
                return pos + length;
                
            } else {
                
                return remaining + recvBufferSize;
                
            }
            
        }
        
    }
    
}

