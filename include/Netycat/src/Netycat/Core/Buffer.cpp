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
#include <cstring>

#include "..\..\..\include\Netycat\Core\Buffer.h"


namespace Netycat {
    
    namespace Core {
        
        Buffer::Buffer(uintptr_t cap) :
            data(new uint8_t[cap]),
            readpos(0),
            writepos(0),
            remaining(0),
            capacity(cap) {
            
        }
        
        Buffer::Buffer(const Buffer& src) :
            data(new uint8_t[src.capacity]),
            readpos(0),
            writepos(src.remaining),
            remaining(src.remaining),
            capacity(src.capacity) {
            
            uintptr_t readpos_ = src.readpos;
            for(uintptr_t i = 0; i < remaining; i++) {
                
                data[i] = src.data[readpos_];
                readpos_++;
                if(readpos_ == capacity) {
                    
                    readpos_ = 0;
                    
                }
                
            }
            
        }
        
        Buffer::~Buffer() {
            
            delete []data;
            
        }
        
        
        bool Buffer::read(void* dat, uintptr_t len) {
            
            if(len > remaining) {
                
                return false;
                
            }
            
            for(uintptr_t i = 0; i < len; i++) {
                
                ((uint8_t*)dat)[i] = data[readpos];
                readpos++;
                if(readpos == capacity) {
                    
                    readpos = 0;
                    
                }
                
            }
            
            remaining -= len;
            
            return true;
            
        }
        
        bool Buffer::readU16(uint16_t* dat, uintptr_t len) {
            
            if(len * 2 > remaining) {
                
                return false;
                
            }
            
            for(uintptr_t i = 0; i < len; i++) {
                
                uint8_t b1;
                uint8_t b2;
                
                b1 = data[readpos];
                readpos++;
                if(readpos == capacity) {
                    
                    readpos = 0;
                    
                }
                
                b2 = data[readpos];
                readpos++;
                if(readpos == capacity) {
                    
                    readpos = 0;
                    
                }
                
                dat[i] = (uint32_t)b1 | ((uint32_t)b2 << 8);
                
            }
            
            remaining -= len * 2;
            
            return true;
            
        }
        
        bool Buffer::readU32(uint32_t* dat, uintptr_t len) {
            
            if(len * 4 > remaining) {
                
                return false;
                
            }
            
            for(uintptr_t i = 0; i < len; i++) {
                
                uint8_t b1;
                uint8_t b2;
                uint8_t b3;
                uint8_t b4;
                
                b1 = data[readpos];
                readpos++;
                if(readpos == capacity) {
                    
                    readpos = 0;
                    
                }
                
                b2 = data[readpos];
                readpos++;
                if(readpos == capacity) {
                    
                    readpos = 0;
                    
                }
                
                b3 = data[readpos];
                readpos++;
                if(readpos == capacity) {
                    
                    readpos = 0;
                    
                }
                
                b4 = data[readpos];
                readpos++;
                if(readpos == capacity) {
                    
                    readpos = 0;
                    
                }
                
                dat[i] = (uint32_t)b1 | ((uint32_t)b2 << 8)
                    | ((uint32_t)b3 << 16) | ((uint32_t)b4 << 24);
                
            }
            
            remaining -= len * 4;
            
            return true;
            
        }
        
        bool Buffer::readBuffer(Buffer& dst, uintptr_t len) {
            
            if(remaining < len) {
                
                return false;
                
            }
            
            if(len + dst.remaining > dst.capacity) {
                
                dst.setCapacity((dst.capacity + len) * 2);
                
            }
            
            for(uintptr_t i = 0; i < len; i++) {
                
                uint8_t b;
                
                b = data[readpos];
                readpos++;
                if(readpos == capacity) {
                    
                    readpos = 0;
                    
                }
                
                dst.data[dst.writepos] = b;
                dst.writepos++;
                if(dst.writepos == dst.capacity) {
                    
                    dst.writepos = 0;
                    
                }
                
            }
            
            remaining -= len;
            dst.remaining += len;
            
            return true;
            
        }
        
        
        bool Buffer::write(const void* dat, uintptr_t len) {
            
            if(len + remaining > capacity) {
                
                setCapacity((capacity + len) * 2);
                
            }
            
            for(uintptr_t i = 0; i < len; i++) {
                
                data[writepos] = ((uint8_t*)dat)[i];
                writepos++;
                if(writepos == capacity) {
                    
                    writepos = 0;
                    
                }
                
            }
            
            remaining += len;
            
            return true;
            
        }
        
        bool Buffer::writeU16(const uint16_t* dat, uintptr_t len) {
            
            if(len * 2 + remaining > capacity) {
                
                setCapacity((capacity + len * 2) * 2);
                
            }
            
            for(uintptr_t i = 0; i < len; i++) {
                
                uint32_t d = dat[i];
                uint8_t b1 = (d & 0x00FF);
                uint8_t b2 = (d & 0xFF00) >> 8;
                
                data[writepos] = b1;
                writepos++;
                if(writepos == capacity) {
                    
                    writepos = 0;
                    
                }
                
                data[writepos] = b2;
                writepos++;
                if(writepos == capacity) {
                    
                    writepos = 0;
                    
                }
                
            }
            
            remaining += len * 2;
            
            return true;
            
        }
        
        bool Buffer::writeU32(const uint32_t* dat, uintptr_t len) {
            
            if(len * 4 + remaining > capacity) {
                
                setCapacity((capacity + len * 4) * 2);
                
            }
            
            for(uintptr_t i = 0; i < len; i++) {
                
                uint32_t d = dat[i];
                uint8_t b1 = (d & 0x000000FF);
                uint8_t b2 = (d & 0x0000FF00) >> 8;
                uint8_t b3 = (d & 0x00FF0000) >> 16;
                uint8_t b4 = (d & 0xFF000000) >> 24;
                
                data[writepos] = b1;
                writepos++;
                if(writepos == capacity) {
                    
                    writepos = 0;
                    
                }
                
                data[writepos] = b2;
                writepos++;
                if(writepos == capacity) {
                    
                    writepos = 0;
                    
                }
                
                data[writepos] = b3;
                writepos++;
                if(writepos == capacity) {
                    
                    writepos = 0;
                    
                }
                
                data[writepos] = b4;
                writepos++;
                if(writepos == capacity) {
                    
                    writepos = 0;
                    
                }
                
            }
            
            remaining += len * 4;
            
            return true;
            
        }
        
        bool Buffer::writeBuffer(Buffer& src, uintptr_t len) {
            
            if(src.remaining < len) {
                
                return false;
                
            }
            
            if(len + remaining > capacity) {
                
                setCapacity((capacity + len) * 2);
                
            }
            
            for(uintptr_t i = 0; i < len; i++) {
                
                uint8_t b;
                
                b = src.data[src.readpos];
                src.readpos++;
                if(src.readpos == src.capacity) {
                    
                    src.readpos = 0;
                    
                }
                
                data[writepos] = b;
                writepos++;
                if(writepos == capacity) {
                    
                    writepos = 0;
                    
                }
                
            }
            
            src.remaining -= len;
            remaining += len;
            
            return true;
            
        }
        
        
        bool Buffer::skip(uintptr_t len) {
            
            if(len > remaining) {
                
                return false;
                
            }
            
            if(capacity - readpos <= len) {
                
                readpos += len - capacity;
                
            } else {
                
                readpos += len;
                
            }
            
            remaining -= len;
            
            return true;
            
        }
        
        
        uint8_t Buffer::at(uintptr_t index) {
            
            if(index >= remaining) {
                
                return 0;
                
            }
            
            if(readpos + index >= capacity) {
                
                return data[readpos + index - capacity];
                
            } else {
                
                return data[readpos + index];
                
            }
            
        }
        
        
        bool Buffer::reset() {
            
            readpos = 0;
            writepos = 0;
            remaining = 0;
            
            return true;
            
        }
        
        
        uintptr_t Buffer::getCapacity() {
            
            return capacity;
            
        }
        
        bool Buffer::setCapacity(uintptr_t cap) {
            
            if(cap < capacity) {
                
                return false;
                
            } else if(cap == capacity) {
                
                return true;
                
            } else {
                
                uint8_t* dat = new uint8_t[cap];
                
                for(uintptr_t i = 0; i < remaining; i++) {
                    
                    dat[i] = data[readpos];
                    readpos++;
                    if(readpos == capacity) {
                        
                        readpos = 0;
                        
                    }
                    
                }
                
                delete []data;
                data = dat;
                capacity = cap;
                readpos = 0;
                writepos = (remaining == capacity) ? 0 : remaining;
                
                return true;
                
            }
            
        }
        
        
        uintptr_t Buffer::getRemaining() {
            
            return remaining;
            
        }
        
        
    }
    
}

