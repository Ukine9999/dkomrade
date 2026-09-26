#pragma once
#include <functional>
#include <string_view>
#include <utility>

#include "Dkomrade/Application/Ports/ILogger.h"

namespace Dkomrade::Infrastructure::Logging
{
    class CallbackLogger final : public Application::Ports::ILogger
    {
    public:
        explicit CallbackLogger(std::function<void(std::string_view)> callback);

        void Log(std::string_view message) override;

    private:
        std::function<void(std::string_view)> m_callback;
    };

    inline CallbackLogger::CallbackLogger(std::function<void(std::string_view)> callback)
        : m_callback(std::move(callback))
    {
    }

    inline void CallbackLogger::Log(std::string_view message)
    {
        if (m_callback)
            m_callback(message);
    }
}
