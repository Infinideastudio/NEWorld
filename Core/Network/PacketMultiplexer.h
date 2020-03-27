#pragma once

#include "Packet.h"

namespace Network {
    class StateBase;

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

        void Transition(std::unique_ptr<StateBase> newState);
    private:
        std::unique_ptr<StateBase> mState;
    };
}