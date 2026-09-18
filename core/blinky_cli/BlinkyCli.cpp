#include "core/blinky_cli/BlinkyCli.hpp"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <optional>

namespace application
{
    namespace
    {
        // strtof needs a terminated buffer, and BoundedConstString is not one.
        // It also reports failure as 0.0, which for a motor command is a valid
        // value, so the parse is only accepted when the whole token was consumed.
        std::optional<float> ParseEffort(const infra::BoundedConstString& token)
        {
            std::array<char, 16> buffer{};
            if (token.empty() || token.size() >= buffer.size())
                return std::nullopt;

            std::copy_n(token.begin(), token.size(), buffer.begin());

            char* end = nullptr;
            const auto value = std::strtof(buffer.data(), &end);

            if (end != buffer.data() + token.size())
                return std::nullopt;

            return value;
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
        , commands{ {
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
          } }
    {}

    infra::MemoryRange<const services::TerminalCommands::Command> BlinkyCli::CliCommands::Commands()
    {
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

        // "<left> <right>", each a signed decimal fraction. Both are parsed before
        // anything is applied, so a malformed command moves neither wheel.
        const auto separator = params.find(' ');
        if (separator == infra::BoundedConstString::npos)
        {
            tracer.Trace() << "usage: drive <left> <right>";
            return;
        }

        const auto effortLeft = ParseEffort(params.substr(0, separator));
        const auto effortRight = ParseEffort(params.substr(separator + 1));

        if (!effortLeft || !effortRight)
        {
            tracer.Trace() << "usage: drive <left> <right>";
            return;
        }

        motionActuation.Apply(*effortLeft, *effortRight);
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
