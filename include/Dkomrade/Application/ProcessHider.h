#pragma once
#include <cstdint>
#include <optional>

#include "Dkomrade/Application/CidEntryEditor.h"
#include "Dkomrade/Application/CidTableWalker.h"
#include "Dkomrade/Application/ListEntryEditor.h"
#include "Dkomrade/Application/Ports/IKernelMemory.h"
#include "Dkomrade/Application/Ports/ILogger.h"
#include "Dkomrade/Domain/HiddenProcess.h"
#include "Dkomrade/Domain/KernelOffsets.h"

namespace Dkomrade::Application
{
    class ProcessHider final
    {
    public:
        ProcessHider(Ports::IKernelMemory& kernelMemory, Ports::ILogger& logger, Domain::KernelOffsets offsets);

        std::optional<Domain::HiddenProcess> Hide(std::uint64_t pid);
        bool Unhide(const Domain::HiddenProcess& hiddenProcess);

    private:
        Ports::IKernelMemory& m_kernelMemory;
        Ports::ILogger& m_logger;
        Domain::KernelOffsets m_offsets;

        CidTableWalker m_cidTableWalker;
        ListEntryEditor m_listEntryEditor;
        CidEntryEditor m_cidEntryEditor;
    };
}
