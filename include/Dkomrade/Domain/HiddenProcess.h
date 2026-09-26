#pragma once
#include <cstdint>
#include <vector>

#include "Dkomrade/Domain/CidUnlinkSnapshot.h"
#include "Dkomrade/Domain/ListUnlinkSnapshot.h"

namespace Dkomrade::Domain
{
    struct HiddenProcess
    {
        std::uint64_t pid = 0;
        std::uint64_t eprocess = 0;

        ListUnlinkSnapshot activeProcessLinks = { };
        ListUnlinkSnapshot sessionProcessLinks = { };
        ListUnlinkSnapshot mmProcessLinks = { };
        ListUnlinkSnapshot jobLinks = { };
        ListUnlinkSnapshot handleTableList = { };

        CidUnlinkSnapshot processCid = { };
        std::vector<CidUnlinkSnapshot> threadCids = { };
    };
}
