#include "Game/Network/Protocol/NoStatePackets.g.h"
#include "Game/Network/States//NoState.h"

namespace Game::Network::Protocol::NoStatePackets {
    void HandShake::Process(States::StateBase& state) {
        auto& noState = state.Down<States::NoState>();
        // TODO: Assert Version Number
        // TODO: Maybe add load balancing support and gateway checking
        noState.Transition(NextState); // Caution: No other line of code is allowed below this line
    }
}