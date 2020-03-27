#include "PacketMultiplexer.h"
#include "PacketReader.h"
#include "StateBase.h"

namespace Network {
    PacketMultiplexer::PacketMultiplexer() { Transition(0); }

    PacketMultiplexer::PacketMultiplexer(PacketMultiplexer&&) noexcept = default;

    PacketMultiplexer& PacketMultiplexer::operator=(PacketMultiplexer&&) noexcept = default;

    PacketMultiplexer::~PacketMultiplexer() noexcept = default;

    void PacketMultiplexer::Inbound(Packet&& packet) {
        PacketReader reader(std::move(packet));
        mState->Handle(reader.VarInt(), reader);
    }

    void PacketMultiplexer::Transition(std::unique_ptr<StateBase> newState) {
        mState = std::move(newState);
    }
}