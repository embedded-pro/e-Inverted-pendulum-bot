#ifndef CORE_CLI_CLI_HPP
#define CORE_CLI_CLI_HPP

#include "core/motion_actuation/interfaces/MotionActuation.hpp"
#include "core/platform_abstraction/Platform.hpp"
#include "services/peripheral/DebugLed.hpp"
#include "services/util/Terminal.hpp"
#include <array>

namespace application
{
    class Cli
    {
    public:
        Cli(platform::Platform& platform, motion::MotionActuation& motionActuation);

    private:
        class CliCommands final
            : public services::TerminalCommands
        {
        public:
            CliCommands(services::TerminalWithCommands& terminal, services::Tracer& tracer, motion::MotionActuation& motionActuation);

            infra::MemoryRange<const Command> Commands() override;

        private:
            void Ping(const infra::BoundedConstString& params);
            void Identify(const infra::BoundedConstString& params);
            void Drive(const infra::BoundedConstString& params);
            void ReleaseBridges(const infra::BoundedConstString& params);
            void Brake(const infra::BoundedConstString& params);

            services::Tracer& tracer;
            motion::MotionActuation& motionActuation;

            // Owned per instance rather than function-local static: the handlers
            // capture this, so a shared array would outlive the object it points at.
            std::array<Command, 5> commands;
        };

        services::DebugLed debugLed;
        services::TerminalWithCommandsImpl::WithMaxQueueAndMaxHistory<> terminal;
        CliCommands commands;
    };
}

#endif
