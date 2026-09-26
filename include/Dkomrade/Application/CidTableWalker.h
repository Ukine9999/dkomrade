#pragma once
#include <cstdint>

#include "Dkomrade/Application/Ports/IKernelMemory.h"
#include "Dkomrade/Application/Ports/ILogger.h"
#include "Dkomrade/Domain/KernelOffsets.h"

namespace Dkomrade::Application
{
    class CidTableWalker final
    {
    public:
        CidTableWalker(Ports::IKernelMemory& kernelMemory, Ports::ILogger& logger, Domain::KernelOffsets offsets);

        std::uint64_t LookupCidEntry(std::uint64_t handle) const;
        std::uint64_t FindEprocessByPid(std::uint64_t pid) const;

    private:
        Ports::IKernelMemory& m_kernelMemory;
        Ports::ILogger& m_logger;
        Domain::KernelOffsets m_offsets;
    };
}
