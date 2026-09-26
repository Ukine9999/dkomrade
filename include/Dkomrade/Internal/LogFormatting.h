#pragma once
#include <cstdint>
#include <format>
#include <string_view>

#include "Dkomrade/Application/Ports/ILogger.h"

namespace Dkomrade::Internal
{
    inline void LogHex(Application::Ports::ILogger& logger, std::string_view name, std::uint64_t value)
    {
        logger.Log(std::format("{:40s}: 0x{:x}", name, value));
    }
}
