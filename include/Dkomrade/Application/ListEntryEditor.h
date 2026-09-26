#pragma once
#include <cstdint>

#include "Dkomrade/Application/Ports/IKernelMemory.h"
#include "Dkomrade/Application/Ports/ILogger.h"
#include "Dkomrade/Domain/ListUnlinkSnapshot.h"

namespace Dkomrade::Application
{
    class ListEntryEditor final
    {
    public:
        ListEntryEditor(Ports::IKernelMemory& kernelMemory, Ports::ILogger& logger);

        bool SnapshotAndUnlink(std::uint64_t nodeAddress, Domain::ListUnlinkSnapshot& snapshot) const;
        bool Restore(const Domain::ListUnlinkSnapshot& snapshot) const;

    private:
        Ports::IKernelMemory& m_kernelMemory;
        Ports::ILogger& m_logger;
    };
}
