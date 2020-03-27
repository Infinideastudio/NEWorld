#include "QueryPackets.g.h"

namespace Game::Protocol::QueryPackets {
    void Request::Process(Network::StateBase& state) {}

    void Ping::Process(Network::StateBase& state) {
        Pong pong {};
        pong.Payload = Payload;
        state.Outbound(pong.Serialize());
    }

    void Response::Process(Network::StateBase& state) {}

    void Pong::Process(Network::StateBase& state) {}
}