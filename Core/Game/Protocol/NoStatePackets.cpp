#include "NoStatePackets.g.h"
#include "Game/Protocol/States/NoStateServer.h"
#include "Game/Protocol/States/QueryStateServer.h"
#include "Game/Protocol/States/LoginStateServer.h"

namespace Game::Protocol::NoStatePackets {
    void HandShake::Process(Network::StateBase& state) {
        auto& noState = state.Down<States::NoStateServer>();
        // TODO: Assert Version Number
        // TODO: Maybe add load balancing support and gateway checking
        switch (NextState) {
        case 0: noState.Transition(std::make_unique<States::NoStateServer>(noState.GetMux()));
            break;
        case 1: noState.Transition(std::make_unique<States::QueryStateServer>(noState.GetMux()));
            break;
        case 2: noState.Transition(std::make_unique<States::LoginStateServer>(noState.GetMux()));
            break;
        default:
            noState.Transition(nullptr);
        }
        // Caution: No other line of code is allowed below this line
    }
}