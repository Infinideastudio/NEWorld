#ifndef _NETYCAT_EXCEPTION_H_
#define _NETYCAT_EXCEPTION_H_


#include <exception>


namespace Netycat {
    
    namespace Core {
        
        class InetAddressException : public std::exception {
            
            public:
            
            const char* what();
            
        };
        
        class SocketException : public std::exception {
            
            public:
            
            const char* what();
            
        };
        
    }
    
}


#endif

