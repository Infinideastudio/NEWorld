#pragma once

#include "Network/StateBase.h"
#include "Game/Protocol/NoStatePackets.g.h"

namespace Game::Protocol::States {
    class NoStateServer : public Network::StateBase {
    public:
        explicit NoStateServer(Network::PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) { }

        void Handle(int id, Network::PacketReader& reader) override {
            Protocol::NoStatePackets::TryHandleServer(id, reader, *this);
        }
    };
}
