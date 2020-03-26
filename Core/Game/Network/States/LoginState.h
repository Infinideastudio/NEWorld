#pragma once

#include "Base.h"
#include "Game/Network/Protocol/LoginPackets.g.h"

namespace Game::Network::States {
    class LoginState: public StateBase {
    public:
        explicit LoginState(PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) {}

        void Handle(int id, PacketReader& reader) override {
            Protocol::LoginPackets::TryHandle(id, reader, *this);
        }
    };
}