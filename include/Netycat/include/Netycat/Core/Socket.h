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


#ifndef _NETYCAT_SOCKET_H_
#define _NETYCAT_SOCKET_H_


#include <cstdint>
#include <memory>
#include <string>

#include "Buffer.h"
#include "BufferCondition.h"
#include "InetAddress.h"


#define SOCKET_BUFFER_SIZE 512


namespace Netycat {
    
    namespace Core {
        
        class Socket {
            
            private:
            
            int sock;
            InetAddress* address;
            
            Buffer recvBuffer;
            uint8_t* recvBufferArray;
            uint8_t* sendBufferArray;
            
            public:
            
            Socket();
            Socket(const Socket& src) = delete;
            Socket(Socket&& src);
            ~Socket();
            
            void connect(InetAddress* addr, uint16_t port);
			void connectIPv4(std::string addr, uint16_t port);
            void listen(InetAddress* addr, uint16_t port);
			void listenIPv4(uint16_t port);
            void accept(Socket& s);
            
            InetAddress* getAddress();
            
            uintptr_t getRecvBufferSize();
            
            uintptr_t getSendBufferSize();
            
            void recv(Buffer& buffer, BufferCondition& condition);
			int recvInt();
			void send(Buffer& buffer, uintptr_t len = 0);
            
            bool close();
            
        };
        
    }
    
}


#endif

