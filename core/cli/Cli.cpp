#include "core/cli/Cli.hpp"
#include "infra/util/Tokenizer.hpp"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <optional>

namespace application
{
    namespace
    {
        std::optional<float> ParseEffort(const infra::BoundedConstString& token)
        {
            std::array<char, 16> buffer{};
            if (token.size() >= buffer.size())
                return std::nullopt;

            std::copy_n(token.begin(), token.size(), buffer.begin());

            char* end = nullptr;
            const auto value = std::strtof(buffer.data(), &end);

            if (end != buffer.data() + token.size())
                return std::nullopt;

            return value;
        }
    }

    Cli::Cli(platform::Platform& platform, motion::MotionActuation& motionActuation)
        : debugLed{ platform.StatusLed() }
        , terminal{ platform.Communication(), platform.Tracer() }
        , commands{ terminal, platform.Tracer(), motionActuation }
    {
        platform.Tracer().Trace() << "inverted-pendulum-bot ready - try 'ping', 'id' or 'drive <left> <right>'";
    }

    Cli::CliCommands::CliCommands(services::TerminalWithCommands& terminal, services::Tracer& tracer, motion::MotionActuation& motionActuation)
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
              { { "tristate", "t", "release both bridges to high impedance" },
                  [this](const infra::BoundedConstString& params)
                  {
                      ReleaseBridges(params);
                  } },
              { { "brake", "b", "short both motors" },
                  [this](const infra::BoundedConstString& params)
                  {
                      Brake(params);
                  } },
          } }
    {}

    infra::MemoryRange<const services::TerminalCommands::Command> Cli::CliCommands::Commands()
    {
        return infra::MakeRange(commands);
    }

    void Cli::CliCommands::Ping(const infra::BoundedConstString&)
    {
        tracer.Trace() << "pong";
    }

    void Cli::CliCommands::Identify(const infra::BoundedConstString&)
    {
        tracer.Trace() << "inverted-pendulum-bot cli";
    }

    void Cli::CliCommands::Drive(const infra::BoundedConstString& params)
    {
        if (motionActuation.Fault() != motion::FaultCause::none)
        {
            tracer.Trace() << "refused: driver fault latched, clear it first";
            return;
        }

        const infra::Tokenizer tokenizer{ params, ' ' };
        std::optional<float> effortLeft;
        std::optional<float> effortRight;

        if (tokenizer.Size() == 2)
        {
            effortLeft = ParseEffort(tokenizer.Token(0));
            effortRight = ParseEffort(tokenizer.Token(1));
        }

        if (!effortLeft.has_value() || !effortRight.has_value())
        {
            tracer.Trace() << "usage: drive <left> <right>";
            return;
        }

        motionActuation.Apply(*effortLeft, *effortRight);
        tracer.Trace() << "driving";
    }

    void Cli::CliCommands::ReleaseBridges(const infra::BoundedConstString&)
    {
        motionActuation.Disable(motion::DisableState::tristate);
        tracer.Trace() << "tristated";
    }

    void Cli::CliCommands::Brake(const infra::BoundedConstString&)
    {
        motionActuation.Disable(motion::DisableState::brake);
        tracer.Trace() << "braking";
    }
}
