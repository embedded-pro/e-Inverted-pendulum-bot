#include "core/cli/Cli.hpp"
#include "core/motion_actuation/interfaces/MotionActuation.hpp"
#include "core/platform_abstraction/test_doubles/PlatformMock.hpp"
#include "hal/interfaces/test_doubles/SerialCommunicationMock.hpp"
#include "infra/stream/StringOutputStream.hpp"
#include "infra/timer/test_helper/ClockFixture.hpp"
#include "services/tracer/Tracer.hpp"
#include "gmock/gmock.h"
#include <string>

namespace
{
    // Minimal GpioPin so DebugLed has a real output to drive in the host test.
    class GpioStub final
        : public hal::GpioPin
    {
    public:
        bool Get() const override
        {
            return false;
        }

        void Set(bool) override
        {}

        bool GetOutputLatch() const override
        {
            return false;
        }

        void SetAsInput() override
        {}

        bool IsInput() const override
        {
            return false;
        }

        void Config(hal::PinConfigType) override
        {}

        void Config(hal::PinConfigType, bool) override
        {}

        void ResetConfig() override
        {}

        void EnableInterrupt(const infra::Function<void()>&, hal::InterruptTrigger, hal::InterruptType) override
        {}

        void DisableInterrupt() override
        {}
    };

    class MotionActuationMock
        : public motion::MotionActuation
    {
    public:
        virtual ~MotionActuationMock() = default;

        MOCK_METHOD(void, Apply, (float effortLeft, float effortRight), (override));
        MOCK_METHOD(void, Disable, (motion::DisableState state), (override));
        MOCK_METHOD(motion::FaultCause, Fault, (), (const, override));
        MOCK_METHOD(void, ClearFault, (), (override));
    };

    class CliTest
        : public testing::Test
        , public infra::ClockFixture
    {
    public:
        CliTest()
        {
            EXPECT_CALL(platform, StatusLed()).WillRepeatedly(testing::ReturnRef(led));
            EXPECT_CALL(platform, Communication()).WillRepeatedly(testing::ReturnRef(communication));
            EXPECT_CALL(platform, Tracer()).WillRepeatedly(testing::ReturnRef(tracer));

            // The terminal echoes every keystroke; the tests assert on the traced
            // output instead, so the echo traffic itself is not interesting.
            EXPECT_CALL(communication, SendDataMock(testing::_)).Times(testing::AnyNumber());
        }

        std::string Output() const
        {
            return std::string{ text.begin(), text.end() };
        }

        void Send(const std::string& line)
        {
            const auto terminated = line + "\r\n";
            const infra::ConstByteRange data{ reinterpret_cast<const uint8_t*>(terminated.data()), reinterpret_cast<const uint8_t*>(terminated.data() + terminated.size()) };

            communication.dataReceived(data);

            // The terminal chains the next write from each completion, so the drain
            // is bounded rather than run until it settles.
            for (int i = 0; i != 64 && communication.actionOnCompletion; ++i)
                communication.actionOnCompletion();

            ExecuteAllActions();
        }

        GpioStub led;
        testing::StrictMock<hal::SerialCommunicationMock> communication;
        infra::BoundedString::WithStorage<512> text;
        infra::StringOutputStream stream{ text };
        services::TracerToStream tracer{ stream };
        testing::StrictMock<platform::PlatformMock> platform;
        testing::StrictMock<MotionActuationMock> motionActuation;
    };
}

TEST_F(CliTest, greets_and_shows_a_prompt_on_construction)
{
    application::Cli cli{ platform, motionActuation };

    EXPECT_THAT(Output(), testing::HasSubstr("ready"));
    EXPECT_THAT(Output(), testing::HasSubstr("drive"));
    EXPECT_THAT(Output(), testing::HasSubstr("> "));
}

TEST_F(CliTest, ping_replies_pong)
{
    application::Cli cli{ platform, motionActuation };

    Send("ping");

    EXPECT_THAT(Output(), testing::HasSubstr("pong"));
}

TEST_F(CliTest, id_prints_the_board_identifier)
{
    application::Cli cli{ platform, motionActuation };

    Send("id");

    EXPECT_THAT(Output(), testing::HasSubstr("inverted-pendulum-bot cli"));
}

TEST_F(CliTest, drive_applies_both_efforts)
{
    application::Cli cli{ platform, motionActuation };

    EXPECT_CALL(motionActuation, Fault()).WillOnce(testing::Return(motion::FaultCause::none));
    EXPECT_CALL(motionActuation, Apply(testing::FloatEq(0.3f), testing::FloatEq(-0.7f)));

    Send("drive 0.3 -0.7");

    EXPECT_THAT(Output(), testing::HasSubstr("driving"));
}

TEST_F(CliTest, drive_without_a_second_argument_prints_usage)
{
    application::Cli cli{ platform, motionActuation };

    EXPECT_CALL(motionActuation, Fault()).WillOnce(testing::Return(motion::FaultCause::none));

    Send("drive 0.3");

    EXPECT_THAT(Output(), testing::HasSubstr("usage: drive"));
}

TEST_F(CliTest, drive_is_refused_while_a_fault_is_latched)
{
    application::Cli cli{ platform, motionActuation };

    EXPECT_CALL(motionActuation, Fault()).WillOnce(testing::Return(motion::FaultCause::driverFault));

    Send("drive 0.3 -0.7");

    EXPECT_THAT(Output(), testing::HasSubstr("refused"));
}

TEST_F(CliTest, tristate_releases_the_bridges)
{
    application::Cli cli{ platform, motionActuation };

    EXPECT_CALL(motionActuation, Disable(motion::DisableState::tristate));

    Send("tristate");

    EXPECT_THAT(Output(), testing::HasSubstr("tristated"));
}

TEST_F(CliTest, brake_shorts_the_motors)
{
    application::Cli cli{ platform, motionActuation };

    EXPECT_CALL(motionActuation, Disable(motion::DisableState::brake));

    Send("brake");

    EXPECT_THAT(Output(), testing::HasSubstr("braking"));
}

// A malformed argument must move neither wheel. strtof reports failure as 0.0,
// which is itself a valid effort, so these would otherwise read as a command to
// stop rather than as a command to reject. The strict mock fails the test on its
// own if Apply is reached.

TEST_F(CliTest, drive_with_a_missing_right_argument_is_rejected)
{
    application::Cli cli{ platform, motionActuation };

    EXPECT_CALL(motionActuation, Fault()).WillOnce(testing::Return(motion::FaultCause::none));

    Send("drive 0.3 ");

    EXPECT_THAT(Output(), testing::HasSubstr("usage: drive"));
}

TEST_F(CliTest, drive_with_a_non_numeric_argument_is_rejected)
{
    application::Cli cli{ platform, motionActuation };

    EXPECT_CALL(motionActuation, Fault()).WillOnce(testing::Return(motion::FaultCause::none));

    Send("drive abc def");

    EXPECT_THAT(Output(), testing::HasSubstr("usage: drive"));
}

TEST_F(CliTest, drive_with_a_trailing_third_argument_is_rejected)
{
    application::Cli cli{ platform, motionActuation };

    EXPECT_CALL(motionActuation, Fault()).WillOnce(testing::Return(motion::FaultCause::none));

    Send("drive 0.3 0.7 0.9");

    EXPECT_THAT(Output(), testing::HasSubstr("usage: drive"));
}

TEST_F(CliTest, drive_with_a_partially_numeric_argument_is_rejected)
{
    application::Cli cli{ platform, motionActuation };

    EXPECT_CALL(motionActuation, Fault()).WillOnce(testing::Return(motion::FaultCause::none));

    Send("drive 0.3 0.7x");

    EXPECT_THAT(Output(), testing::HasSubstr("usage: drive"));
}

TEST_F(CliTest, drive_with_an_over_long_argument_is_rejected)
{
    application::Cli cli{ platform, motionActuation };

    EXPECT_CALL(motionActuation, Fault()).WillOnce(testing::Return(motion::FaultCause::none));

    Send("drive 0.3 0.70000000000000");

    EXPECT_THAT(Output(), testing::HasSubstr("usage: drive"));
}
