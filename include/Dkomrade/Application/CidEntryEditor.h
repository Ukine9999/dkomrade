#pragma once
#include <cstdint>

#include "Dkomrade/Application/Ports/IKernelMemory.h"
#include "Dkomrade/Application/Ports/ILogger.h"
#include "Dkomrade/Domain/CidUnlinkSnapshot.h"

namespace Dkomrade::Application
{
    class CidEntryEditor final
    {
    public:
        CidEntryEditor(Ports::IKernelMemory& kernelMemory, Ports::ILogger& logger);

        bool SnapshotAndClear(std::uint64_t entryAddress, Domain::CidUnlinkSnapshot& snapshot) const;
        bool Restore(const Domain::CidUnlinkSnapshot& snapshot) const;

    private:
        Ports::IKernelMemory& m_kernelMemory;
        Ports::ILogger& m_logger;
    };
}
