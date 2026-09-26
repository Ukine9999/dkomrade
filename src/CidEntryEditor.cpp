#include "Dkomrade/Application/CidEntryEditor.h"

#include "Dkomrade/Internal/LogFormatting.h"

namespace Dkomrade::Application
{
    CidEntryEditor::CidEntryEditor(Ports::IKernelMemory& kernelMemory, Ports::ILogger& logger)
        : m_kernelMemory(kernelMemory)
        , m_logger(logger)
    {
    }

    bool CidEntryEditor::SnapshotAndClear(std::uint64_t entryAddress, Domain::CidUnlinkSnapshot& snapshot) const
    {
        snapshot.taken = false;
        Internal::LogHex(m_logger, "clearCid entryAddress", entryAddress);

        if (!entryAddress)
        {
            m_logger.Log("clearCid abort: entryAddress zero");
            return false;
        }

        const auto lowOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, entryAddress + 0);
        const auto highOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, entryAddress + 8);
        if (!lowOpt || !highOpt)
        {
            m_logger.Log("clearCid abort: read low or high failed");
            return false;
        }
        const std::uint64_t low = *lowOpt;
        const std::uint64_t high = *highOpt;
        Internal::LogHex(m_logger, "clearCid low", low);
        Internal::LogHex(m_logger, "clearCid high", high);

        if (!low)
        {
            m_logger.Log("clearCid abort: low zero");
            return false;
        }

        snapshot.entryAddress = entryAddress;
        snapshot.savedLow = low;
        snapshot.savedHigh = high;
        snapshot.taken = true;

        bool ok = true;
        if (!Ports::WriteValue<std::uint64_t>(m_kernelMemory, entryAddress + 0, std::uint64_t(0)))
        {
            m_logger.Log("clearCid: write low reported failure");
            ok = false;
        }
        if (!Ports::WriteValue<std::uint64_t>(m_kernelMemory, entryAddress + 8, std::uint64_t(0)))
        {
            m_logger.Log("clearCid: write high reported failure");
            ok = false;
        }

        if (ok)
            m_logger.Log("clearCid ok");
        return ok;
    }

    bool CidEntryEditor::Restore(const Domain::CidUnlinkSnapshot& snapshot) const
    {
        if (!snapshot.taken)
        {
            m_logger.Log("restoreCid skip: not taken");
            return true;
        }

        Internal::LogHex(m_logger, "restoreCid entryAddress", snapshot.entryAddress);
        Internal::LogHex(m_logger, "restoreCid savedLow", snapshot.savedLow);
        Internal::LogHex(m_logger, "restoreCid savedHigh", snapshot.savedHigh);

        bool ok = true;
        ok = Ports::WriteValue<std::uint64_t>(m_kernelMemory, snapshot.entryAddress + 0, snapshot.savedLow) && ok;
        ok = Ports::WriteValue<std::uint64_t>(m_kernelMemory, snapshot.entryAddress + 8, snapshot.savedHigh) && ok;
        if (!ok)
            m_logger.Log("restoreCid: one or more writes failed");
        else
            m_logger.Log("restoreCid ok");
        return ok;
    }
}
