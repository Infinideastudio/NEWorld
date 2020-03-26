#pragma once

#include "Base.h"
#include "Game/Network/Protocol/QueryPackets.g.h"

namespace Game::Network::States {
    class QueryState : public StateBase {
    public:
        explicit QueryState(PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) { }

        void Handle(int id, PacketReader& reader) override {
            Protocol::QueryPackets::TryHandle(id, reader, *this);
        }
    };
}
