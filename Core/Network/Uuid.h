#pragma once

#include <cstdint>
#include "PacketReader.h"
#include "PacketWriter.h"

namespace Network {
    struct Uuid {
        uint64_t lo, hi;

        [[nodiscard]] int SerializedSize() const noexcept { return 0; }

        void Serialize(PacketWriter& writer) const noexcept {
            // TODO: implement this
        }

        void Deserialize(PacketReader& reader) {
            // TODO: implement this
        }
    };
}
