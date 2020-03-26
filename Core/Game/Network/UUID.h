#pragma once

#include <cstdint>

namespace Game::Network {
    struct UUID {
        uint64_t lo, hi;
    };
}
