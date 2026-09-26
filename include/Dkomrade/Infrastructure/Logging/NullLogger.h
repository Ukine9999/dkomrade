#pragma once
#include <string_view>

#include "Dkomrade/Application/Ports/ILogger.h"

namespace Dkomrade::Infrastructure::Logging
{
    class NullLogger final : public Application::Ports::ILogger
    {
    public:
        void Log(std::string_view) override
        {
        }
    };
}
