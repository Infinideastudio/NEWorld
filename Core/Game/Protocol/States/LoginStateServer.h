#pragma once

#include "Network/StateBase.h"
#include "Game/Protocol/LoginPackets.g.h"

namespace Game::Protocol::States {
    class LoginStateServer: public Network::StateBase {
    public:
        explicit LoginStateServer(Network::PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) {}

        void Handle(int id, Network::PacketReader& reader) override {
            Protocol::LoginPackets::TryHandleServer(id, reader, *this);
        }
    };
}