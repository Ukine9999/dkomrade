#pragma once
#include <cstdint>

namespace Dkomrade::Domain
{
    struct CidUnlinkSnapshot
    {
        std::uint64_t entryAddress = 0;
        std::uint64_t savedLow = 0;
        std::uint64_t savedHigh = 0;
        bool taken = false;
    };
}
