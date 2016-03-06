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
#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "..\..\..\include\Netycat\Core\Exception.h"
#include "..\..\..\include\Netycat\Core\InetAddress.h"


namespace Netycat {
    
    namespace Core {
        
        InetAddress::InetAddress() {
            
        }
        
        InetAddress::InetAddress(const InetAddress&) {
            
        }
        
        InetAddress::~InetAddress() {
            
        }
        
		std::shared_ptr<InetAddress> InetAddress::getByName(const char *host) {
            
            if(host == nullptr) {
                
                throw InetAddressException();
                
            }
            
            struct addrinfo hints;
            struct addrinfo* result;
            int ret;
            
            ::memset(&hints, 0, sizeof(hints));
            hints.ai_flags = AI_PASSIVE;
            hints.ai_family = AF_UNSPEC;
            
            if((ret = ::getaddrinfo(host, nullptr, &hints, &result)) != 0) {
                
                throw InetAddressException();
                
            }
            
            if(result == nullptr) {
                
                throw InetAddressException();
                
            }
            
            if(result->ai_addr == nullptr) {
                
                throw InetAddressException();
                
            }
            
            switch(result->ai_addr->sa_family) {
                
                case AF_INET: {
                    
                    struct sockaddr_in* saddr
                        = (struct sockaddr_in*)result->ai_addr;
                    
                    uint32_t addr = ntohl(saddr->sin_addr.s_addr);
                    
                    uint8_t d1 = (addr & 0xFF000000) >> 24;
                    uint8_t d2 = (addr & 0x00FF0000) >> 16;
                    uint8_t d3 = (addr & 0x0000FF00) >> 8;
                    uint8_t d4 = (addr & 0x000000FF);
                    
                    freeaddrinfo(result);
                    
					return std::shared_ptr<InetAddress>(new InetAddressIPv4(d1, d2, d3, d4));
                    
                }
                case AF_INET6: {
                    
                    struct sockaddr_in6* saddr
                        = (struct sockaddr_in6*)result->ai_addr;
                    
                    uint8_t d1 = saddr->sin6_addr.s6_addr[0];
                    uint8_t d2 = saddr->sin6_addr.s6_addr[1];
                    uint8_t d3 = saddr->sin6_addr.s6_addr[2];
                    uint8_t d4 = saddr->sin6_addr.s6_addr[3];
                    uint8_t d5 = saddr->sin6_addr.s6_addr[4];
                    uint8_t d6 = saddr->sin6_addr.s6_addr[5];
                    uint8_t d7 = saddr->sin6_addr.s6_addr[6];
                    uint8_t d8 = saddr->sin6_addr.s6_addr[7];
                    uint8_t d9 = saddr->sin6_addr.s6_addr[8];
                    uint8_t d10 = saddr->sin6_addr.s6_addr[9];
                    uint8_t d11 = saddr->sin6_addr.s6_addr[10];
                    uint8_t d12 = saddr->sin6_addr.s6_addr[11];
                    uint8_t d13 = saddr->sin6_addr.s6_addr[12];
                    uint8_t d14 = saddr->sin6_addr.s6_addr[13];
                    uint8_t d15 = saddr->sin6_addr.s6_addr[14];
                    uint8_t d16 = saddr->sin6_addr.s6_addr[15];
                    
                    freeaddrinfo(result);
                    
					return  std::shared_ptr<InetAddress>(new InetAddressIPv6(
						d1, d2, d3, d4, d5, d6, d7, d8,
						d9, d10, d11, d12, d13, d14, d15, d16));
                    
                }
                default: {
                    
                    freeaddrinfo(result);
                    
                    throw InetAddressException();
                    
                    
                }
                
            }
            
            /*
            struct hostent* hostentData = ::gethostbyname(host);
            if(hostentData == nullptr) {
                
                throw InetAddressException();
                
            }
                    
            switch(hostentData->h_addrtype) {
                
                case AF_INET: {
                    
                    if(hostentData->h_addr_list[0] == nullptr) {
                
                        throw InetAddressException();
                        
                    }
                    
                    uint8_t d1 = hostentData->h_addr_list[0][0];
                    uint8_t d2 = hostentData->h_addr_list[0][1];
                    uint8_t d3 = hostentData->h_addr_list[0][2];
                    uint8_t d4 = hostentData->h_addr_list[0][3];
                    
                    return new InetAddressIPv4(d1, d2, d3, d4);
                    
                }
                default: {
                
                    throw InetAddressException();
                    
                }
                
            }
            */
            
        }
        
        
        InetAddressIPv4::InetAddressIPv4() {
            
            data[0] = 0;
            data[1] = 0;
            data[2] = 0;
            data[3] = 0;
            
        }
        
        InetAddressIPv4::InetAddressIPv4(uint8_t d1, uint8_t d2,
            uint8_t d3, uint8_t d4) {
            
            data[0] = d1;
            data[1] = d2;
            data[2] = d3;
            data[3] = d4;
            
        }
        
        InetAddressIPv4::InetAddressIPv4(const InetAddressIPv4& src) {
            
            data[0] = src.data[0];
            data[1] = src.data[1];
            data[2] = src.data[2];
            data[3] = src.data[3];
            
        }
        
        InetAddressIPv4::~InetAddressIPv4() {
            
        }
        
        
        InetAddress::InetAddressType InetAddressIPv4::getType() {
            
            return IPv4;
            
        }
        
        
        std::string InetAddressIPv4::toString() {
            
            std::string res;
            
            for(uintptr_t i = 0; i < 4; i++) {
                
                uint8_t d = data[i];
                
                if(d >= 100) {
                    
                    res += '0' + d / 100;
                    res += '0' + (d / 10) % 10;
                    res += '0' + d % 10;
                    
                } else if(d >= 10) {
                    
                    res += '0' + d / 10;
                    res += '0' + d % 10;
                    
                } else {
                    
                    res += '0' + d;
                    
                }
                
                if(i != 4 - 1) {
                    
                    res += '.';
                    
                }
                
            }
            
            return res;
            
        }
        
        
        uint8_t* InetAddressIPv4::getData() {
            
            return data;
            
        }
        
        
		std::shared_ptr<InetAddress> InetAddressIPv4::getAny() {
            
            return std::shared_ptr<InetAddress>(new InetAddressIPv4(0, 0, 0, 0));
            
        }
        
        InetAddressIPv4* InetAddressIPv4::getLoopback() {
            
            return new InetAddressIPv4(127, 0, 0, 1);
            
        }
        
        
        InetAddressIPv6::InetAddressIPv6() {
            
            data[0] = 0;
            data[1] = 0;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            data[8] = 0;
            data[9] = 0;
            data[10] = 0;
            data[11] = 0;
            data[12] = 0;
            data[13] = 0;
            data[14] = 0;
            data[15] = 0;
            
        }
        
        InetAddressIPv6::InetAddressIPv6(uint8_t d1, uint8_t d2,
            uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6,
            uint8_t d7, uint8_t d8, uint8_t d9, uint8_t d10,
            uint8_t d11, uint8_t d12, uint8_t d13, uint8_t d14,
            uint8_t d15, uint8_t d16) {
            
            data[0] = d1;
            data[1] = d2;
            data[2] = d3;
            data[3] = d4;
            data[4] = d5;
            data[5] = d6;
            data[6] = d7;
            data[7] = d8;
            data[8] = d9;
            data[9] = d10;
            data[10] = d11;
            data[11] = d12;
            data[12] = d13;
            data[13] = d14;
            data[14] = d15;
            data[15] = d16;
            
        }
        
        InetAddressIPv6::InetAddressIPv6(const InetAddressIPv6& src) {
            
            data[0] = src.data[0];
            data[1] = src.data[1];
            data[2] = src.data[2];
            data[3] = src.data[3];
            data[4] = src.data[4];
            data[5] = src.data[5];
            data[6] = src.data[6];
            data[7] = src.data[7];
            data[8] = src.data[8];
            data[9] = src.data[9];
            data[10] = src.data[10];
            data[11] = src.data[11];
            data[12] = src.data[12];
            data[13] = src.data[13];
            data[14] = src.data[14];
            data[15] = src.data[15];
            
        }
        
        InetAddressIPv6::~InetAddressIPv6() {
            
        }
        
        
        InetAddress::InetAddressType InetAddressIPv6::getType() {
            
            return IPv6;
            
        }
        
        
        std::string InetAddressIPv6::toString() {
            
            const char hexTable[] = "0123456789abcdef";
            
            std::string res;
            
            for(uintptr_t i = 0; i < 16; i += 2) {
                
                uint8_t d1 = data[i];
                uint8_t d2 = data[i + 1];
                
                if(d1 >= 0x10) {
                    
                    res += hexTable[d1 >> 4];
                    res += hexTable[d1 & 0x0F];
                    res += hexTable[d2 >> 4];
                    res += hexTable[d2 & 0x0F];
                    
                } else if(d1 > 0x00) {
                    
                    res += hexTable[d1];
                    res += hexTable[d2 >> 4];
                    res += hexTable[d2 & 0x0F];
                    
                } else if(d2 >= 0x10) {
                    
                    res += hexTable[d2 >> 4];
                    res += hexTable[d2 & 0x0F];
                    
                } else {
                    
                    res += hexTable[d2];
                    
                }
                
                if(i != 16 - 2) {
                    
                    res += ':';
                    
                }
                
            }
            
            return res;
            
        }
        
        
        uint8_t* InetAddressIPv6::getData() {
            
            return data;
            
        }
        
        
		std::shared_ptr<InetAddress> InetAddressIPv6::getAny() {
            
            return std::shared_ptr<InetAddress>(new InetAddressIPv6(0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0));
            
        }
        
        InetAddressIPv6* InetAddressIPv6::getLoopback() {
            
            return new InetAddressIPv6(0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 1);
            
        }
        
    }
    
}

