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

#include <winsock2.h>
#include <ws2tcpip.h>

#include "..\..\..\include\Netycat\Core\Buffer.h"
#include "..\..\..\include\Netycat\Core\BufferCondition.h"
#include "..\..\..\include\Netycat\Core\Exception.h"
#include "..\..\..\include\Netycat\Core\InetAddress.h"
#include "..\..\..\include\Netycat\Core\Socket.h"


namespace Netycat {
    
    namespace Core {
        
        Socket::Socket() :
            sock(-1),
            address(nullptr),
            recvBuffer(SOCKET_BUFFER_SIZE),
            recvBufferArray(new uint8_t[SOCKET_BUFFER_SIZE]),
            sendBufferArray(new uint8_t[SOCKET_BUFFER_SIZE]) {
            
        }
        
        Socket::Socket(Socket&& src) :
            sock(src.sock),
            address(src.address),
            recvBuffer(src.recvBuffer),
            recvBufferArray(new uint8_t[SOCKET_BUFFER_SIZE]),
            sendBufferArray(new uint8_t[SOCKET_BUFFER_SIZE]) {
            
            src.sock = -1;
            src.address = nullptr;
            
        }
        
        Socket::~Socket() {
            
            if(sock != -1) {
                
                ::closesocket(sock);
                
            }
            
            if(address != nullptr) {
                
                delete address;
                
            }
            
            delete []recvBufferArray;
            delete []sendBufferArray;
            
        }
        
        
        void Socket::connect(InetAddress* addr, uint16_t port) {
            
            if(sock != -1) {
                
                throw SocketException();
                
            }
            
            if(!addr) {
                
                throw SocketException();
                
            }
            
            switch(addr->getType()) {
                
                case InetAddress::IPv4: {
                    
                    InetAddressIPv4* addrIPv4 = (InetAddressIPv4*)addr;
                    uint8_t* dataIPv4 = addrIPv4->getData();
                    
                    sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
                    if(sock == -1) {
                        
                        throw SocketException();
                        
                    }
                    
                    struct sockaddr_in saddr;
                    ::memset(&saddr, 0, sizeof(saddr));
                    saddr.sin_family = AF_INET;
                    saddr.sin_port = htons(port);
                    saddr.sin_addr.s_addr = htonl(
                        (dataIPv4[0] << 24) | (dataIPv4[1] << 16) |
                        (dataIPv4[2] << 8) | dataIPv4[3]);
                    
                    if(::connect(sock, (struct sockaddr*)&saddr, sizeof(saddr))
                        != 0) {
                        
                        ::closesocket(sock);
                        sock = -1;
                        
                        throw SocketException();
                        
                    }
                    
                    address = new InetAddressIPv4(*addrIPv4);
                    
                    break;
                    
                }
                case InetAddress::IPv6: {
                    
                    InetAddressIPv6* addrIPv6 = (InetAddressIPv6*)addr;
                    uint8_t* dataIPv6 = addrIPv6->getData();
                    
                    sock = ::socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
                    if(sock == -1) {
                        
                        throw SocketException();
                        
                    }
                    
                    struct sockaddr_in6 saddr;
                    ::memset(&saddr, 0, sizeof(saddr));
                    saddr.sin6_family = AF_INET6;
                    saddr.sin6_port = htons(port);
                    saddr.sin6_addr.s6_addr[0] = dataIPv6[0];
                    saddr.sin6_addr.s6_addr[1] = dataIPv6[1];
                    saddr.sin6_addr.s6_addr[2] = dataIPv6[2];
                    saddr.sin6_addr.s6_addr[3] = dataIPv6[3];
                    saddr.sin6_addr.s6_addr[4] = dataIPv6[4];
                    saddr.sin6_addr.s6_addr[5] = dataIPv6[5];
                    saddr.sin6_addr.s6_addr[6] = dataIPv6[6];
                    saddr.sin6_addr.s6_addr[7] = dataIPv6[7];
                    saddr.sin6_addr.s6_addr[8] = dataIPv6[8];
                    saddr.sin6_addr.s6_addr[9] = dataIPv6[9];
                    saddr.sin6_addr.s6_addr[10] = dataIPv6[10];
                    saddr.sin6_addr.s6_addr[11] = dataIPv6[11];
                    saddr.sin6_addr.s6_addr[12] = dataIPv6[12];
                    saddr.sin6_addr.s6_addr[13] = dataIPv6[13];
                    saddr.sin6_addr.s6_addr[14] = dataIPv6[14];
                    saddr.sin6_addr.s6_addr[15] = dataIPv6[15];
                    
                    if(::connect(sock, (struct sockaddr*)&saddr, sizeof(saddr))
                        != 0) {
                        
                        ::closesocket(sock);
                        sock = -1;
                        
                        throw SocketException();
                        
                    }
                    
                    address = new InetAddressIPv6(*addrIPv6);
                    
                    break;
                    
                }
                default: {
                    
                    throw SocketException();
                    
                }
                
            }
            
        }

		void Socket::connectIPv4(std::string addr, uint16_t port) {
			connect(InetAddress::getByName(addr.c_str()).get(), port);
		}

        void Socket::listen(InetAddress* addr, uint16_t port) {
            
            if(sock != -1) {
                
                throw SocketException();
                
            }
            
            if(!addr) {
                
                throw SocketException();
                
            }
            
            switch(addr->getType()) {
                
                case InetAddress::IPv4: {
                    
                    InetAddressIPv4* addrIPv4 = (InetAddressIPv4*)addr;
                    uint8_t* dataIPv4 = addrIPv4->getData();
                    
                    sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
                    if(sock == -1) {
                        
                        throw SocketException();
                        
                    }
                    
                    struct sockaddr_in saddr;
                    ::memset(&saddr, 0, sizeof(saddr));
                    saddr.sin_family = AF_INET;
                    saddr.sin_port = htons(port);
                    saddr.sin_addr.s_addr = htonl(
                        (dataIPv4[0] << 24) | (dataIPv4[1] << 16) |
                        (dataIPv4[2] << 8) | dataIPv4[3]);
                    
                    if(::bind(sock, (struct sockaddr*)&saddr, sizeof(saddr))
                        != 0) {
                        
                        ::closesocket(sock);
                        sock = -1;
                        
                        throw SocketException();
                        
                    }
                    
                    if(::listen(sock, 8) != 0) {
                        
                        ::closesocket(sock);
                        sock = -1;
                        
                        throw SocketException();
                        
                    }
                    
                    address = new InetAddressIPv4(*addrIPv4);
                    
                    break;
                    
                }
                case InetAddress::IPv6: {
                    
                    InetAddressIPv6* addrIPv6 = (InetAddressIPv6*)addr;
                    uint8_t* dataIPv6 = addrIPv6->getData();
                    
                    sock = ::socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
                    if(sock == -1) {
                        
                        throw SocketException();
                        
                    }
                    
                    struct sockaddr_in6 saddr;
                    ::memset(&saddr, 0, sizeof(saddr));
                    saddr.sin6_family = AF_INET6;
                    saddr.sin6_port = htons(port);
                    saddr.sin6_addr.s6_addr[0] = dataIPv6[0];
                    saddr.sin6_addr.s6_addr[1] = dataIPv6[1];
                    saddr.sin6_addr.s6_addr[2] = dataIPv6[2];
                    saddr.sin6_addr.s6_addr[3] = dataIPv6[3];
                    saddr.sin6_addr.s6_addr[4] = dataIPv6[4];
                    saddr.sin6_addr.s6_addr[5] = dataIPv6[5];
                    saddr.sin6_addr.s6_addr[6] = dataIPv6[6];
                    saddr.sin6_addr.s6_addr[7] = dataIPv6[7];
                    saddr.sin6_addr.s6_addr[8] = dataIPv6[8];
                    saddr.sin6_addr.s6_addr[9] = dataIPv6[9];
                    saddr.sin6_addr.s6_addr[10] = dataIPv6[10];
                    saddr.sin6_addr.s6_addr[11] = dataIPv6[11];
                    saddr.sin6_addr.s6_addr[12] = dataIPv6[12];
                    saddr.sin6_addr.s6_addr[13] = dataIPv6[13];
                    saddr.sin6_addr.s6_addr[14] = dataIPv6[14];
                    saddr.sin6_addr.s6_addr[15] = dataIPv6[15];
                    
                    if(::bind(sock, (struct sockaddr*)&saddr, sizeof(saddr))
                        != 0) {
                        
                        ::closesocket(sock);
                        sock = -1;
                        
                        throw SocketException();
                        
                    }
                    
                    if(::listen(sock, 8) != 0) {
                        
                        ::closesocket(sock);
                        sock = -1;
                        
                        throw SocketException();
                        
                    }
                    
                    address = new InetAddressIPv6(*addrIPv6);
                    
                    break;
                    
                }
                default: {
                    
                    throw SocketException();
                    
                }
                
            }
            
		}
		
		void Socket::listenIPv4(uint16_t port) {
			listen(InetAddressIPv4::getAny().get(), port);
		}

        void Socket::accept(Socket& s) {
            
            if(sock == -1) {
                
                throw SocketException();
                
            }
            if(s.sock != -1) {
                
                throw SocketException();
                
            }
            
            
            switch(address->getType()) {
                
                case InetAddress::IPv4: {
                    
                    struct sockaddr_in saddr;
                    int slen = sizeof(saddr);
                    
                    if((s.sock = ::accept(sock, (sockaddr*)&saddr, &slen))
                        == -1) {
                        
                        printf("%d\n", WSAGetLastError());
                        throw SocketException();
                        
                    }
                    
                    uint32_t addr = ntohl(saddr.sin_addr.s_addr);
                    
                    uint8_t d1 = (addr & 0xFF000000) >> 24;
                    uint8_t d2 = (addr & 0x00FF0000) >> 16;
                    uint8_t d3 = (addr & 0x0000FF00) >> 8;
                    uint8_t d4 = (addr & 0x000000FF);
                    
                    s.address = new InetAddressIPv4(d1, d2, d3, d4);
                    
                    break;
                    
                }
                case InetAddress::IPv6: {
                    
                    struct sockaddr_in6 saddr;
                    int slen = sizeof(saddr);
                    
                    if((s.sock = ::accept(sock, (sockaddr*)&saddr, &slen))
                        == -1) {
                        
                        printf("%d\n", WSAGetLastError());
                        throw SocketException();
                        
                    }
                    
                    uint8_t d1 = saddr.sin6_addr.s6_addr[0];
                    uint8_t d2 = saddr.sin6_addr.s6_addr[1];
                    uint8_t d3 = saddr.sin6_addr.s6_addr[2];
                    uint8_t d4 = saddr.sin6_addr.s6_addr[3];
                    uint8_t d5 = saddr.sin6_addr.s6_addr[4];
                    uint8_t d6 = saddr.sin6_addr.s6_addr[5];
                    uint8_t d7 = saddr.sin6_addr.s6_addr[6];
                    uint8_t d8 = saddr.sin6_addr.s6_addr[7];
                    uint8_t d9 = saddr.sin6_addr.s6_addr[8];
                    uint8_t d10 = saddr.sin6_addr.s6_addr[9];
                    uint8_t d11 = saddr.sin6_addr.s6_addr[10];
                    uint8_t d12 = saddr.sin6_addr.s6_addr[11];
                    uint8_t d13 = saddr.sin6_addr.s6_addr[12];
                    uint8_t d14 = saddr.sin6_addr.s6_addr[13];
                    uint8_t d15 = saddr.sin6_addr.s6_addr[14];
                    uint8_t d16 = saddr.sin6_addr.s6_addr[15];
                    
                    s.address = new InetAddressIPv6(
                        d1, d2, d3, d4, d5, d6, d7, d8,
                        d9, d10, d11, d12, d13, d14, d15, d16);
                    
                    break;
                    
                }
                default: {
                    
                    throw SocketException();
                    
                }
                
            }
            
        }
        
        
        InetAddress* Socket::getAddress() {
            
            return address;
            
        }
                
        
        uintptr_t Socket::getRecvBufferSize() {
            
            if(sock == -1) {
                
                throw SocketException();
                
            }
            
            int val;
            int len = sizeof(val);
            ::getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&val, &len);
            
            return (uintptr_t)val;
            
        }
        
        
        uintptr_t Socket::getSendBufferSize() {
            
            if(sock == -1) {
                
                throw SocketException();
                
            }
            
            int val;
            int len = sizeof(val);
            ::getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&val, &len);
            
            return (uintptr_t)val;
            
        }
        
        
        void Socket::recv(Buffer& buffer, BufferCondition& condition) {
            
            uintptr_t len;
            
            while(recvBuffer.getRemaining() < (len = condition.control(recvBuffer, SOCKET_BUFFER_SIZE))) {
                
                int ret;
                ret = ::recv(sock, (char*)recvBufferArray, len - recvBuffer.getRemaining(), 0);
                if(ret == SOCKET_ERROR) {
                    
                    throw SocketException();
                    
                }
                if(ret == 0) {
                    
                    throw SocketException();
                    
                }
                
                recvBuffer.write(recvBufferArray, ret);
                
            }
            
            buffer.writeBuffer(recvBuffer, len);
            
        }

		int Socket::recvInt() {
			Buffer buffer(4);
			recv(buffer, BufferConditionExactLength(sizeof(int)));
			int ret;
			buffer.read((void*)&ret, sizeof(int));
			return ret;
		}

        void Socket::send(Buffer& buffer, uintptr_t len) {
            
			if (len == 0) len = buffer.getRemaining();

            if(len > buffer.getRemaining()) {
                
                throw SocketException();
                
            }
            
            while(len >= SOCKET_BUFFER_SIZE) {
                
                buffer.read(sendBufferArray, SOCKET_BUFFER_SIZE);
                
                int ret;
                ret = ::send(sock, (char*)sendBufferArray, SOCKET_BUFFER_SIZE, 0);
                if(ret == SOCKET_ERROR) {
                    
                    throw SocketException();
                    
                }
                
                len -= SOCKET_BUFFER_SIZE;
                
            }
            
            if(len != 0) {
                
                buffer.read(sendBufferArray, buffer.getRemaining());
                
                int ret;
                ret = ::send(sock, (char*)sendBufferArray, len, 0);
                if(ret == SOCKET_ERROR) {
                    
                    throw SocketException();
                    
                }
                
            }
        }
        
        
        bool Socket::close() {
            
            if(sock != -1) {
                
                ::closesocket(sock);
                sock = -1;
                
                return true;
                
            } else {
                
                throw SocketException();
                
            }
            
        }
        
    }
    
}

