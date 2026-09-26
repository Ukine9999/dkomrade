#pragma once
#include <cstdint>

namespace Dkomrade::Domain
{
    struct ListUnlinkSnapshot
    {
        std::uint64_t nodeAddress = 0;
        std::uint64_t savedFlink = 0;
        std::uint64_t savedBlink = 0;
        bool taken = false;
    };
}
