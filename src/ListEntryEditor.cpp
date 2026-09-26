#include "Dkomrade/Application/ListEntryEditor.h"

#include "Dkomrade/Internal/LogFormatting.h"

namespace Dkomrade::Application
{
    ListEntryEditor::ListEntryEditor(Ports::IKernelMemory& kernelMemory, Ports::ILogger& logger)
        : m_kernelMemory(kernelMemory)
        , m_logger(logger)
    {
    }

    bool ListEntryEditor::SnapshotAndUnlink(std::uint64_t nodeAddress, Domain::ListUnlinkSnapshot& snapshot) const
    {
        snapshot.taken = false;
        snapshot.nodeAddress = nodeAddress;
        Internal::LogHex(m_logger, "unlink nodeAddress", nodeAddress);

        const auto flinkOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, nodeAddress + 0);
        const auto blinkOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, nodeAddress + 8);
        if (!flinkOpt || !blinkOpt)
        {
            m_logger.Log("unlink abort: read flink or blink failed");
            return false;
        }
        snapshot.savedFlink = *flinkOpt;
        snapshot.savedBlink = *blinkOpt;
        Internal::LogHex(m_logger, "unlink savedFlink", snapshot.savedFlink);
        Internal::LogHex(m_logger, "unlink savedBlink", snapshot.savedBlink);

        if (!snapshot.savedFlink || !snapshot.savedBlink)
        {
            m_logger.Log("unlink abort: flink or blink zero");
            return false;
        }

        snapshot.taken = true;

        bool ok = true;
        if (!Ports::WriteValue<std::uint64_t>(m_kernelMemory, snapshot.savedBlink + 0, snapshot.savedFlink))
        {
            m_logger.Log("unlink: write blink.flink reported failure");
            ok = false;
        }
        if (!Ports::WriteValue<std::uint64_t>(m_kernelMemory, snapshot.savedFlink + 8, snapshot.savedBlink))
        {
            m_logger.Log("unlink: write flink.blink reported failure");
            ok = false;
        }
        if (!Ports::WriteValue<std::uint64_t>(m_kernelMemory, nodeAddress + 0, nodeAddress))
        {
            m_logger.Log("unlink: write self.flink reported failure");
            ok = false;
        }
        if (!Ports::WriteValue<std::uint64_t>(m_kernelMemory, nodeAddress + 8, nodeAddress))
        {
            m_logger.Log("unlink: write self.blink reported failure");
            ok = false;
        }

        if (ok)
            m_logger.Log("unlink ok");
        return ok;
    }

    bool ListEntryEditor::Restore(const Domain::ListUnlinkSnapshot& snapshot) const
    {
        if (!snapshot.taken)
        {
            m_logger.Log("restoreList skip: not taken");
            return true;
        }

        Internal::LogHex(m_logger, "restoreList nodeAddress", snapshot.nodeAddress);
        Internal::LogHex(m_logger, "restoreList savedFlink", snapshot.savedFlink);
        Internal::LogHex(m_logger, "restoreList savedBlink", snapshot.savedBlink);

        const auto liveBlinkFlinkOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, snapshot.savedBlink + 0);
        const auto liveFlinkBlinkOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, snapshot.savedFlink + 8);
        if (!liveBlinkFlinkOpt || !liveFlinkBlinkOpt)
        {
            m_logger.Log("restoreList abort: read neighbors failed");
            return false;
        }
        const std::uint64_t liveBlinkFlink = *liveBlinkFlinkOpt;
        const std::uint64_t liveFlinkBlink = *liveFlinkBlinkOpt;
        Internal::LogHex(m_logger, "restoreList liveBlinkFlink", liveBlinkFlink);
        Internal::LogHex(m_logger, "restoreList liveFlinkBlink", liveFlinkBlink);

        if (liveBlinkFlink != snapshot.savedFlink || liveFlinkBlink != snapshot.savedBlink)
        {
            m_logger.Log("restoreList abort: neighbors changed since hide");
            return false;
        }

        bool ok = true;
        ok = Ports::WriteValue<std::uint64_t>(m_kernelMemory, snapshot.nodeAddress + 0, snapshot.savedFlink) && ok;
        ok = Ports::WriteValue<std::uint64_t>(m_kernelMemory, snapshot.nodeAddress + 8, snapshot.savedBlink) && ok;
        ok = Ports::WriteValue<std::uint64_t>(m_kernelMemory, snapshot.savedBlink + 0, snapshot.nodeAddress) && ok;
        ok = Ports::WriteValue<std::uint64_t>(m_kernelMemory, snapshot.savedFlink + 8, snapshot.nodeAddress) && ok;
        if (!ok)
            m_logger.Log("restoreList: one or more writes failed");
        else
            m_logger.Log("restoreList ok");
        return ok;
    }
}
