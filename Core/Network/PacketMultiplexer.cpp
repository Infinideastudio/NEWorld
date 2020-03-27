#include "PacketMultiplexer.h"
#include "Network/States/NoStateServer.h"
#include "Network/States/QueryStateServer.h"
#include "Network/States/LoginStateServer.h"
#include "Network/States/GameStateServer.h"

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
        case 0: mState = std::make_unique<States::NoStateServer>(this);
            break;
        case 1: mState = std::make_unique<States::QueryStateServer>(this);
            break;
        case 2: mState = std::make_unique<States::LoginStateServer>(this);
            break;
        case 3: mState = std::make_unique<States::GameStateServer>(this);
            break;
        default: mState.reset();
        }
    }
}