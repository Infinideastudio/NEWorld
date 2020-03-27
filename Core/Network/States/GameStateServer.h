#pragma once

#include "Base.h"
#include "Network/Protocol/GamePackets.g.h"

namespace Game::Network::States {
    class GameStateServer: public StateBase {
    public:
        explicit GameStateServer(PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) { }

        void Handle(int id, PacketReader& reader) override {
            Protocol::GamePackets::TryHandleServer(id, reader, *this);
        }
    };
}