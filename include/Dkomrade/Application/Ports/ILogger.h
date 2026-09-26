#pragma once
#include <string_view>

namespace Dkomrade::Application::Ports
{
    class ILogger
    {
    public:
        virtual ~ILogger() = default;

        virtual void Log(std::string_view message) = 0;
    };
}
