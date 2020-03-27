#pragma once

#include "Base.h"
#include "Game/Network/Protocol/QueryPackets.g.h"

namespace Game::Network::States {
    class QueryStateServer : public StateBase {
    public:
        explicit QueryStateServer(PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) { }

        void Handle(int id, PacketReader& reader) override {
            Protocol::QueryPackets::TryHandleServer(id, reader, *this);
        }
    };
}
