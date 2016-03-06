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


#ifndef _NETYCAT_INETADDRESS_H_
#define _NETYCAT_INETADDRESS_H_


#include <cstdint>
#include <memory>
#include <string>


namespace Netycat {
    
    namespace Core {
        
        class InetAddress {
            
            public:
            
            enum InetAddressType {
                
                IPv4,
                IPv6,
                
            };
            
            public:
            
            InetAddress();
            InetAddress(const InetAddress& src);
            virtual ~InetAddress() = 0;
            
            virtual InetAddressType getType() = 0;
            
            virtual std::string toString() = 0;
            
            public:
            
            static std::shared_ptr<InetAddress> getByName(const char *host);
            
        };
        
        class InetAddressIPv4 : public InetAddress {
            
            private:
            
            uint8_t data[4];
            
            public:
            
            InetAddressIPv4();
            InetAddressIPv4(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4);
            InetAddressIPv4(const InetAddressIPv4& src);
            ~InetAddressIPv4();
            
            InetAddress::InetAddressType getType();
            
            std::string toString();
            
            uint8_t* getData();
            
            public:
            
            static std::shared_ptr<InetAddress> getAny();
            static InetAddressIPv4* getLoopback();
            
        };
        
        class InetAddressIPv6 : public InetAddress {
            
            private:
            
            uint8_t data[16];
            
            public:
            
            InetAddressIPv6();
            InetAddressIPv6(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4,
                uint8_t d5, uint8_t d6, uint8_t d7, uint8_t d8,
                uint8_t d9, uint8_t d10, uint8_t d11, uint8_t d12,
                uint8_t d13, uint8_t d14, uint8_t d15, uint8_t d16);
            InetAddressIPv6(const InetAddressIPv6& src);
            ~InetAddressIPv6();
            
            InetAddress::InetAddressType getType();
            
            std::string toString();
            
            uint8_t* getData();
            
            public:
            
            static std::shared_ptr<InetAddress> getAny();
            static InetAddressIPv6* getLoopback();
            
        };
        
    }
    
}


#endif

