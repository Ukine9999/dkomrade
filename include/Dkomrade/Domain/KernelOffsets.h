#pragma once
#include <cstdint>

namespace Dkomrade::Domain
{
    struct KernelOffsets
    {
        std::uint64_t psActiveProcessHead = 0;
        std::uint64_t pspCidTable = 0;
        std::uint64_t obHeaderCookie = 0;
        std::uint64_t obTypeIndexTable = 0;

        std::uint32_t eprocessActiveProcessLinks = 0;
        std::uint32_t eprocessUniqueProcessId = 0;
        std::uint32_t eprocessSessionProcessLinks = 0;
        std::uint32_t eprocessMmProcessLinks = 0;
        std::uint32_t eprocessJobLinks = 0;
        std::uint32_t eprocessObjectTable = 0;
        std::uint32_t eprocessThreadListHead = 0;
        std::uint32_t eprocessImageFileName = 0;
        std::uint32_t eprocessImageFileNameSize = 15;

        std::uint32_t ethreadCid = 0;
        std::uint32_t ethreadThreadListEntry = 0;

        std::uint32_t clientIdUniqueThread = 0;

        std::uint32_t handleTableNextHandle = 0;
        std::uint32_t handleTableTableCode = 0;
        std::uint32_t handleTableHandleList = 0;
        std::uint32_t handleTableUniqueProcessId = 0;

        std::uint32_t handleEntrySize = 16;
        std::uint32_t objectHeaderBodyOffset = 0;
        std::uint32_t objectHeaderTypeIndexOffset = 0;
    };
}
