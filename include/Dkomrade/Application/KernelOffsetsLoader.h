#pragma once
#include <cstdint>
#include <optional>

#include "Dkomrade/Application/Ports/ISymbolProvider.h"
#include "Dkomrade/Domain/KernelOffsets.h"

namespace Dkomrade::Application
{
    class KernelOffsetsLoader final
    {
    public:
        explicit KernelOffsetsLoader(Ports::ISymbolProvider& symbolProvider);

        std::optional<Domain::KernelOffsets> Load(std::uint64_t ntKernelBase) const;

    private:
        Ports::ISymbolProvider& m_symbolProvider;
    };
}
