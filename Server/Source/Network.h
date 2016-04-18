#pragma once
#include <Winsock2.h>
#pragma comment(lib, "ws2_32.lib")

namespace Network
{
	const unsigned short port = 30001;

	inline SOCKET getServerSocket();

	SOCKET waitForClient();

	void cleanUp();

	void init();
}
