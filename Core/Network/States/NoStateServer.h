#pragma once

#include "Base.h"
#include "Network/Protocol/NoStatePackets.g.h"

namespace Game::Network::States {
    class NoStateServer : public StateBase {
    public:
        explicit NoStateServer(PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) { }

        void Handle(int id, PacketReader& reader) override {
            Protocol::NoStatePackets::TryHandleServer(id, reader, *this);
        }
    };
}
