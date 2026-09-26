#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>

namespace Dkomrade::Application::Ports
{
    class IKernelMemory
    {
    public:
        virtual ~IKernelMemory() = default;

        virtual bool Read(std::uint64_t address, void* destination, std::size_t size) = 0;
        virtual bool Write(std::uint64_t address, const void* source, std::size_t size) = 0;
    };

    template<class TValue>
    std::optional<TValue> ReadValue(IKernelMemory& kernelMemory, std::uint64_t address)
    {
        TValue value = TValue{ };
        if (!kernelMemory.Read(address, &value, sizeof(TValue)))
            return std::nullopt;
        return value;
    }

    template<class TValue>
    bool WriteValue(IKernelMemory& kernelMemory, std::uint64_t address, TValue value)
    {
        return kernelMemory.Write(address, &value, sizeof(TValue));
    }
}
