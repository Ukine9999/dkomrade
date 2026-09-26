#include "Dkomrade/Application/KernelOffsetsLoader.h"

#include <string_view>

namespace Dkomrade::Application
{
    KernelOffsetsLoader::KernelOffsetsLoader(Ports::ISymbolProvider& symbolProvider)
        : m_symbolProvider(symbolProvider)
    {
    }

    std::optional<Domain::KernelOffsets> KernelOffsetsLoader::Load(std::uint64_t ntKernelBase) const
    {
        if (!ntKernelBase)
            return std::nullopt;

        const std::string_view moduleName = "system32\\ntoskrnl.exe";

        auto offsets = Domain::KernelOffsets{ };
        bool ok = true;

        auto loadGlobal = [&](std::string_view symbolName, std::uint64_t& destination)
        {
            const std::uint64_t rva = m_symbolProvider.GetOffset(moduleName, symbolName);
            if (!rva)
            {
                ok = false;
                return;
            }
            destination = ntKernelBase + rva;
        };

        auto loadField = [&](std::string_view symbolName, std::uint32_t& destination)
        {
            const std::uint64_t offset = m_symbolProvider.GetOffset(moduleName, symbolName);
            if (!offset)
            {
                ok = false;
                return;
            }
            destination = static_cast<std::uint32_t>(offset);
        };

        loadGlobal("PsActiveProcessHead", offsets.psActiveProcessHead);
        loadGlobal("PspCidTable", offsets.pspCidTable);
        loadGlobal("ObHeaderCookie", offsets.obHeaderCookie);
        loadGlobal("ObTypeIndexTable", offsets.obTypeIndexTable);

        loadField("_EPROCESS::ActiveProcessLinks", offsets.eprocessActiveProcessLinks);
        loadField("_EPROCESS::UniqueProcessId", offsets.eprocessUniqueProcessId);
        loadField("_EPROCESS::SessionProcessLinks", offsets.eprocessSessionProcessLinks);
        loadField("_EPROCESS::MmProcessLinks", offsets.eprocessMmProcessLinks);
        loadField("_EPROCESS::JobLinks", offsets.eprocessJobLinks);
        loadField("_EPROCESS::ObjectTable", offsets.eprocessObjectTable);
        loadField("_EPROCESS::ThreadListHead", offsets.eprocessThreadListHead);
        loadField("_EPROCESS::ImageFileName", offsets.eprocessImageFileName);

        loadField("_ETHREAD::Cid", offsets.ethreadCid);
        loadField("_ETHREAD::ThreadListEntry", offsets.ethreadThreadListEntry);

        loadField("_CLIENT_ID::UniqueThread", offsets.clientIdUniqueThread);

        loadField("_HANDLE_TABLE::TableCode", offsets.handleTableTableCode);
        loadField("_HANDLE_TABLE::HandleTableList", offsets.handleTableHandleList);
        loadField("_HANDLE_TABLE::UniqueProcessId", offsets.handleTableUniqueProcessId);

        loadField("_OBJECT_HEADER::Body", offsets.objectHeaderBodyOffset);
        loadField("_OBJECT_HEADER::TypeIndex", offsets.objectHeaderTypeIndexOffset);

        if (!ok)
            return std::nullopt;

        return offsets;
    }
}
