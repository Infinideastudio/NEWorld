#pragma once

#include "Network/PacketReader.h"
#include "Network/PacketMultiplexer.h"

namespace Network {
    class StateBase {
    public:
        explicit StateBase(PacketMultiplexer* mMuxRef) noexcept:mMuxRef(mMuxRef) { }

        virtual ~StateBase() noexcept = default;

        virtual void Handle(int id, PacketReader& reader) = 0;

        void Outbound(Packet packet) { mMuxRef->Outbound(std::move(packet)); }

        void Transition(std::unique_ptr<StateBase> newState) { mMuxRef->Transition(std::move(newState)); }

        template <class T>
        T& Down() { return static_cast<T&>(*this); } // NOLINT

        [[nodiscard]] auto GetMux() const noexcept { return mMuxRef; }
    private:
        PacketMultiplexer* mMuxRef;
    };
}