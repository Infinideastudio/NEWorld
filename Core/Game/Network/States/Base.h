#pragma once

#include "Game/Network/PacketReader.h"
#include "Game/Network/PacketMultiplexer.h"

namespace Game::Network::States {
    class StateBase {
    public:
        explicit StateBase(Game::Network::PacketMultiplexer* mMuxRef) noexcept:mMuxRef(mMuxRef) { }

        virtual ~StateBase() noexcept = default;

        virtual void Handle(int id, PacketReader& reader) = 0;

        void Outbound(Packet packet) { mMuxRef->Outbound(std::move(packet)); }

        void Transition(int next) { mMuxRef->Transition(next); }

        template <class T>
        T& Down() { return static_cast<T&>(*this); } // NOLINT
    private:
        Game::Network::PacketMultiplexer* mMuxRef;
    };
}