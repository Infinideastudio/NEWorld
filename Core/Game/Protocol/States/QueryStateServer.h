#pragma once

#include "Network/StateBase.h"
#include "Game/Protocol/QueryPackets.g.h"

namespace Game::Protocol::States {
    class QueryStateServer : public Network::StateBase {
    public:
        explicit QueryStateServer(Network::PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) { }

        void Handle(int id, Network::PacketReader& reader) override {
            Protocol::QueryPackets::TryHandleServer(id, reader, *this);
        }
    };
}
