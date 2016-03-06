#include "Network.h"

namespace Network {
	Net::Socket serverSocket;
	void cleanUp() {
		Net::cleanup();
	}
	void init() {
		Net::startup();
		serverSocket.listenIPv4(30001);
	}
}