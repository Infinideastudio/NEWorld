#include "NoStatePackets.g.h"
#include "Network/States/NoStateServer.h"

namespace Game::Network::Protocol::NoStatePackets {
    void HandShake::Process(States::StateBase& state) {
        auto& noState = state.Down<States::NoStateServer>();
        // TODO: Assert Version Number
        // TODO: Maybe add load balancing support and gateway checking
        noState.Transition(NextState); // Caution: No other line of code is allowed below this line
    }
}