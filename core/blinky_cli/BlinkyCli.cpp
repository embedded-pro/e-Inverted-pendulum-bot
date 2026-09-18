#include "core/blinky_cli/BlinkyCli.hpp"
#include <algorithm>
#include <array>
#include <cstdlib>

namespace application
{
    namespace
    {
        // strtof needs a terminated buffer, and BoundedConstString is not one.
        float ParseEffort(const infra::BoundedConstString& token)
        {
            std::array<char, 16> buffer{};
            const auto size = std::min(token.size(), buffer.size() - 1);
            std::copy_n(token.begin(), size, buffer.begin());

            return std::strtof(buffer.data(), nullptr);
        }
    }

    BlinkyCli::BlinkyCli(platform::Platform& platform, motion::MotionActuation& motionActuation)
        : debugLed{ platform.StatusLed() }
        , terminal{ platform.Communication(), platform.Tracer() }
        , commands{ terminal, platform.Tracer(), motionActuation }
    {
        platform.Tracer().Trace() << "inverted-pendulum-bot ready - try 'ping', 'id' or 'drive <left> <right>'";
    }

    BlinkyCli::CliCommands::CliCommands(services::TerminalWithCommands& terminal, services::Tracer& tracer, motion::MotionActuation& motionActuation)
        : services::TerminalCommands(terminal)
        , tracer(tracer)
        , motionActuation(motionActuation)
    {}

    infra::MemoryRange<const services::TerminalCommands::Command> BlinkyCli::CliCommands::Commands()
    {
        static const std::array<Command, 5> commands = { {
            { { "ping", "p", "reply with pong" },
                [this](const infra::BoundedConstString& params)
                {
                    Ping(params);
                } },
            { { "id", "i", "print the board identifier" },
                [this](const infra::BoundedConstString& params)
                {
                    Identify(params);
                } },
            { { "drive", "d", "apply signed effort to both wheels, -1.0 to 1.0" },
                [this](const infra::BoundedConstString& params)
                {
                    Drive(params);
                } },
            { { "coast", "c", "release both bridges" },
                [this](const infra::BoundedConstString& params)
                {
                    Coast(params);
                } },
            { { "brake", "b", "short both motors" },
                [this](const infra::BoundedConstString& params)
                {
                    Brake(params);
                } },
        } };

        return infra::MakeRange(commands);
    }

    void BlinkyCli::CliCommands::Ping(const infra::BoundedConstString&)
    {
        tracer.Trace() << "pong";
    }

    void BlinkyCli::CliCommands::Identify(const infra::BoundedConstString&)
    {
        tracer.Trace() << "inverted-pendulum-bot blinky-cli";
    }

    void BlinkyCli::CliCommands::Drive(const infra::BoundedConstString& params)
    {
        if (motionActuation.Fault() != motion::FaultCause::none)
        {
            tracer.Trace() << "refused: driver fault latched, clear it first";
            return;
        }

        // "<left> <right>", each a signed decimal fraction.
        const auto separator = params.find(' ');
        if (separator == infra::BoundedConstString::npos)
        {
            tracer.Trace() << "usage: drive <left> <right>";
            return;
        }

        motionActuation.Apply(ParseEffort(params.substr(0, separator)), ParseEffort(params.substr(separator + 1)));
        tracer.Trace() << "driving";
    }

    void BlinkyCli::CliCommands::Coast(const infra::BoundedConstString&)
    {
        motionActuation.Disable(motion::DisableState::coast);
        tracer.Trace() << "coasting";
    }

    void BlinkyCli::CliCommands::Brake(const infra::BoundedConstString&)
    {
        motionActuation.Disable(motion::DisableState::brake);
        tracer.Trace() << "braking";
    }
}
