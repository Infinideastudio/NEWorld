#pragma once

#include "Network/StateBase.h"
#include "Game/Protocol/GamePackets.g.h"

namespace Game::Protocol::States {
    class GameStateServer : public Network::StateBase {
    public:
        explicit GameStateServer(Network::PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) { }

        void Handle(int id, Network::PacketReader& reader) override {
            Protocol::GamePackets::TryHandleServer(id, reader, *this);
        }
    };
}