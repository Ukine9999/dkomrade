#include "Dkomrade/Application/CidTableWalker.h"

#include "Dkomrade/Internal/LogFormatting.h"

namespace Dkomrade::Application
{
    CidTableWalker::CidTableWalker(Ports::IKernelMemory& kernelMemory, Ports::ILogger& logger, Domain::KernelOffsets offsets)
        : m_kernelMemory(kernelMemory)
        , m_logger(logger)
        , m_offsets(offsets)
    {
    }

    std::uint64_t CidTableWalker::LookupCidEntry(std::uint64_t handle) const
    {
        Internal::LogHex(m_logger, "cidLookup handle", handle);
        Internal::LogHex(m_logger, "cidLookup pspCidTable address", m_offsets.pspCidTable);
        Internal::LogHex(m_logger, "cidLookup nextHandle offset", m_offsets.handleTableNextHandle);
        Internal::LogHex(m_logger, "cidLookup tableCode offset", m_offsets.handleTableTableCode);

        const auto cidTableOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, m_offsets.pspCidTable);
        if (!cidTableOpt)
        {
            m_logger.Log("cidLookup abort: read pspCidTable failed");
            return 0;
        }
        const std::uint64_t cidTable = *cidTableOpt;
        Internal::LogHex(m_logger, "cidLookup pspCidTable deref", cidTable);
        if (!cidTable)
        {
            m_logger.Log("cidLookup abort: pspCidTable deref zero");
            return 0;
        }

        const auto capOpt = Ports::ReadValue<std::uint32_t>(m_kernelMemory, cidTable + m_offsets.handleTableNextHandle);
        if (!capOpt)
        {
            m_logger.Log("cidLookup abort: read cap failed");
            return 0;
        }
        const auto codeOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, cidTable + m_offsets.handleTableTableCode);
        if (!codeOpt)
        {
            m_logger.Log("cidLookup abort: read code failed");
            return 0;
        }
        const std::uint32_t cap = *capOpt;
        const std::uint64_t code = *codeOpt;
        Internal::LogHex(m_logger, "cidLookup cap", cap);
        Internal::LogHex(m_logger, "cidLookup code", code);

        if (!code || !cap)
        {
            m_logger.Log("cidLookup abort: code or cap zero");
            return 0;
        }

        const std::uint64_t base = code & ~std::uint64_t(3);
        const std::uint64_t levels = code & 3;
        const std::uint64_t index = handle & ~std::uint64_t(3);
        Internal::LogHex(m_logger, "cidLookup base", base);
        Internal::LogHex(m_logger, "cidLookup levels", levels);
        Internal::LogHex(m_logger, "cidLookup index", index);

        if (index >= cap)
        {
            m_logger.Log("cidLookup abort: index >= cap");
            return 0;
        }

        if (levels == 0)
        {
            const std::uint64_t entry = base + 4 * index;
            Internal::LogHex(m_logger, "cidLookup entry lvl0", entry);
            return entry;
        }

        if (levels == 1)
        {
            const std::uint64_t lowTableAddress = base + 8 * (index >> 10);
            const auto lowTableOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, lowTableAddress);
            Internal::LogHex(m_logger, "cidLookup lowTableAddress lvl1", lowTableAddress);
            if (!lowTableOpt)
            {
                m_logger.Log("cidLookup abort lvl1: read lowTable failed");
                return 0;
            }
            const std::uint64_t lowTable = *lowTableOpt;
            Internal::LogHex(m_logger, "cidLookup lowTable lvl1", lowTable);
            if (!lowTable)
            {
                m_logger.Log("cidLookup abort lvl1: lowTable zero");
                return 0;
            }
            const std::uint64_t entry = lowTable + 4 * (index & 0x3FF);
            Internal::LogHex(m_logger, "cidLookup entry lvl1", entry);
            return entry;
        }

        const std::uint64_t midTableAddress = base + 8 * (index >> 19);
        const auto midTableOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, midTableAddress);
        Internal::LogHex(m_logger, "cidLookup midTableAddress lvl2", midTableAddress);
        if (!midTableOpt)
        {
            m_logger.Log("cidLookup abort lvl2: read midTable failed");
            return 0;
        }
        const std::uint64_t midTable = *midTableOpt;
        Internal::LogHex(m_logger, "cidLookup midTable lvl2", midTable);
        if (!midTable)
        {
            m_logger.Log("cidLookup abort lvl2: midTable zero");
            return 0;
        }

        const std::uint64_t lowTableAddress = midTable + 8 * ((index >> 10) & 0x1FF);
        const auto lowTableOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, lowTableAddress);
        Internal::LogHex(m_logger, "cidLookup lowTableAddress lvl2", lowTableAddress);
        if (!lowTableOpt)
        {
            m_logger.Log("cidLookup abort lvl2: read lowTable failed");
            return 0;
        }
        const std::uint64_t lowTable = *lowTableOpt;
        Internal::LogHex(m_logger, "cidLookup lowTable lvl2", lowTable);
        if (!lowTable)
        {
            m_logger.Log("cidLookup abort lvl2: lowTable zero");
            return 0;
        }

        const std::uint64_t entry = lowTable + 4 * (index & 0x3FF);
        Internal::LogHex(m_logger, "cidLookup entry lvl2", entry);
        return entry;
    }

    std::uint64_t CidTableWalker::FindEprocessByPid(std::uint64_t pid) const
    {
        Internal::LogHex(m_logger, "findEprocess pid", pid);
        const std::uint64_t entry = LookupCidEntry(pid);
        Internal::LogHex(m_logger, "findEprocess entry", entry);
        if (!entry)
        {
            m_logger.Log("findEprocess abort: entry zero");
            return 0;
        }

        const auto lowOpt = Ports::ReadValue<std::uint64_t>(m_kernelMemory, entry);
        if (!lowOpt)
        {
            m_logger.Log("findEprocess abort: read entry.low failed");
            return 0;
        }
        const std::uint64_t low = *lowOpt;
        Internal::LogHex(m_logger, "findEprocess entry.low", low);
        if (!low)
        {
            m_logger.Log("findEprocess abort: low zero");
            return 0;
        }

        std::int64_t object = static_cast<std::int64_t>(low) >> 16;
        object &= ~std::int64_t(0xF);
        const std::uint64_t eprocess = static_cast<std::uint64_t>(object);
        Internal::LogHex(m_logger, "findEprocess eprocess", eprocess);
        return eprocess;
    }
}
