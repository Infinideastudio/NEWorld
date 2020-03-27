#pragma once

#include "Base.h"
#include "Game/Network/Protocol/LoginPackets.g.h"

namespace Game::Network::States {
    class LoginStateServer: public StateBase {
    public:
        explicit LoginStateServer(PacketMultiplexer* mMuxRef) noexcept
                :StateBase(mMuxRef) {}

        void Handle(int id, PacketReader& reader) override {
            Protocol::LoginPackets::TryHandleServer(id, reader, *this);
        }
    };
}