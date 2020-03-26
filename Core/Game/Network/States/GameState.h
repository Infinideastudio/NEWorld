#pragma once

#include "Base.h"
#include "Game/Network/Protocol/GamePackets.g.h"

namespace Game::Network::States {
    class GameState: public StateBase {
    public:
        explicit GameState(PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) { }

        void Handle(int id, PacketReader& reader) override {
            Protocol::GamePackets::TryHandle(id, reader, *this);
        }
    };
}