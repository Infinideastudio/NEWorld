#include "QueryPackets.g.h"

namespace Game::Network::Protocol::QueryPackets {
    void Request::Process(States::StateBase& state) {}

    void Ping::Process(States::StateBase& state) {
        Pong pong {};
        pong.Payload = Payload;
        state.Outbound(pong.Serialize());
    }

    void Response::Process(States::StateBase& state) {}

    void Pong::Process(States::StateBase& state) {}
}