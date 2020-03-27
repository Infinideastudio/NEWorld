#pragma once

#include "Packet.h"

namespace Game::Network {
    namespace States { class StateBase; }

    class PacketMultiplexer {
    public:
        PacketMultiplexer();

        PacketMultiplexer(const PacketMultiplexer&) = delete;

        PacketMultiplexer& operator = (const PacketMultiplexer&) = delete;

        PacketMultiplexer(PacketMultiplexer&&) noexcept;

        PacketMultiplexer& operator = (PacketMultiplexer&&) noexcept;

        virtual ~PacketMultiplexer() noexcept;

        void Inbound(Packet&& packet);

        virtual void Outbound(Packet&& packet) = 0;

        void Transition(int mode);
    private:
        std::unique_ptr<States::StateBase> mState;
    };
}