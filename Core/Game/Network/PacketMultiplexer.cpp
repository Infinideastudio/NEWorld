#include "PacketMultiplexer.h"
#include "Game/Network/States/NoState.h"
#include "Game/Network/States/QueryState.h"
#include "Game/Network/States/LoginState.h"
#include "Game/Network/States/GameState.h"

namespace Game::Network {
    PacketMultiplexer::PacketMultiplexer() { Transition(0); }

    PacketMultiplexer::PacketMultiplexer(PacketMultiplexer&&) noexcept = default;

    PacketMultiplexer& PacketMultiplexer::operator=(PacketMultiplexer&&) noexcept = default;

    PacketMultiplexer::~PacketMultiplexer() noexcept = default;

    void PacketMultiplexer::Inbound(Packet&& packet) {
        PacketReader reader(std::move(packet));
        mState->Handle(reader.VarInt(), reader);
    }

    void PacketMultiplexer::Transition(int mode) {
        switch (mode) {
        case 0: mState = std::make_unique<States::NoState>(this);
            break;
        case 1: mState = std::make_unique<States::QueryState>(this);
            break;
        case 2: mState = std::make_unique<States::LoginState>(this);
            break;
        case 3: mState = std::make_unique<States::GameState>(this);
            break;
        default: mState.reset();
        }
    }
}