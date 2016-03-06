#include <exception>

#include "..\..\..\include\Netycat\Core\Exception.h"


namespace Netycat {
    
    namespace Core {
        
        const char* InetAddressException::what() {
            
            return "InetAddressException";
            
        }
        
        const char* SocketException::what() {
            
            return "SocketException";
            
        }
        
    }
    
}
