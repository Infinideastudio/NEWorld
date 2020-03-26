#pragma once

#include "Base.h"
#include "Game/Network/Protocol/NoStatePackets.g.h"

namespace Game::Network::States {
    class NoState : public StateBase {
    public:
        explicit NoState(PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) { }

        void Handle(int id, PacketReader& reader) override {
            Protocol::NoStatePackets::TryHandle(id, reader, *this);
        }
    };
}
