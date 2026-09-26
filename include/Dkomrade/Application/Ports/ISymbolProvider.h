#pragma once
#include <cstdint>
#include <string_view>

namespace Dkomrade::Application::Ports
{
    class ISymbolProvider
    {
    public:
        virtual ~ISymbolProvider() = default;

        virtual std::uint64_t GetOffset(std::string_view moduleName, std::string_view symbolName) = 0;
    };
}
