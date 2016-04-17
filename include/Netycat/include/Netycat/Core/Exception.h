/*******************************************************************************

    Copyright 2015 SuperSodaSea
    (C) Copyright 2016 DLaboratory

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


#ifndef _NETYCAT_EXCEPTION_H_
#define _NETYCAT_EXCEPTION_H_


#include <exception>

namespace Netycat
{

namespace Core
{

class InetAddressException : public std::exception
{
public:
    const char* what();
};

class SocketException : public std::exception
{
public:
    const char* what();

};

}

}

#endif

