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

#ifdef NEWORLD_TARGET_WINDOWS
#include <winsock2.h>
#endif

#include "../../../include/Netycat/Core/Core.h"


namespace Netycat
{

namespace Core
{

bool startup()
{
#ifdef NEWORLD_TARGET_WINDOWS
    WSADATA wsaData;
    return WSAStartup(WINSOCK_VERSION, &wsaData) == 0;
#elif NEWORLD_TARGET_MACOSX
    return true;
#endif
}

bool cleanup()
{
#ifdef NEWORLD_TARGET_WINDOWS
    WSACleanup();
#endif
    return true;
}

}

}

