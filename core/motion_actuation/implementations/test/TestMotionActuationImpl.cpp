#include "core/motion_actuation/implementations/MotionActuationImpl.hpp"
#include "core/platform_abstraction/test_doubles/MotorBridgeMock.hpp"
#include "hal/interfaces/test_doubles/GpioMock.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <optional>

namespace
{
    class MotionActuationImplTest
        : public testing::Test
    {
    public:
        MotionActuationImplTest()
        {
            EXPECT_CALL(left, SetBaseFrequency(hal::Hertz{ 25000 }));
            EXPECT_CALL(right, SetBaseFrequency(hal::Hertz{ 25000 }));
            EXPECT_CALL(left, Stop());
            EXPECT_CALL(right, Stop());
            EXPECT_CALL(faultPin, EnableInterrupt(testing::_, hal::InterruptTrigger::fallingEdge, testing::_))
                .WillOnce(testing::SaveArg<0>(&onFault));

            actuation.emplace(left, right, faultPin);
        }

        ~MotionActuationImplTest() override
        {
            EXPECT_CALL(faultPin, DisableInterrupt());
            EXPECT_CALL(left, Stop());
            EXPECT_CALL(right, Stop());
            actuation = std::nullopt;
        }

        testing::StrictMock<platform::MotorBridgeMock> left;
        testing::StrictMock<platform::MotorBridgeMock> right;
        testing::StrictMock<hal::GpioPinMock> faultPin;
        infra::Function<void()> onFault;
        std::optional<motion::MotionActuationImpl> actuation;
    };

    // Construction is left to each test so the configuration can vary.
    class MotionActuationImplConfigTest
        : public testing::Test
    {
    public:
        ~MotionActuationImplConfigTest() override
        {
            if (actuation)
            {
                EXPECT_CALL(faultPin, DisableInterrupt());
                EXPECT_CALL(left, Stop());
                EXPECT_CALL(right, Stop());
                actuation = std::nullopt;
            }
        }

        testing::StrictMock<platform::MotorBridgeMock> left;
        testing::StrictMock<platform::MotorBridgeMock> right;
        testing::StrictMock<hal::GpioPinMock> faultPin;
        std::optional<motion::MotionActuationImpl> actuation;
    };
}

TEST_F(MotionActuationImplTest, positive_effort_drives_half_bridge_a)
{
    EXPECT_CALL(left, Start(hal::Percent{ 30 }, hal::Percent{ 0 }));
    EXPECT_CALL(right, Start(hal::Percent{ 70 }, hal::Percent{ 0 }));

    actuation->Apply(0.3f, 0.7f);
}

TEST_F(MotionActuationImplTest, negative_effort_drives_half_bridge_b)
{
    EXPECT_CALL(left, Start(hal::Percent{ 0 }, hal::Percent{ 30 }));
    EXPECT_CALL(right, Start(hal::Percent{ 0 }, hal::Percent{ 70 }));

    actuation->Apply(-0.3f, -0.7f);
}

TEST_F(MotionActuationImplTest, opposite_efforts_turn_the_robot)
{
    EXPECT_CALL(left, Start(hal::Percent{ 30 }, hal::Percent{ 0 }));
    EXPECT_CALL(right, Start(hal::Percent{ 0 }, hal::Percent{ 30 }));

    actuation->Apply(0.3f, -0.3f);
}

TEST_F(MotionActuationImplTest, effort_beyond_the_range_is_clamped_not_wrapped)
{
    EXPECT_CALL(left, Start(hal::Percent{ 100 }, hal::Percent{ 0 }));
    EXPECT_CALL(right, Start(hal::Percent{ 0 }, hal::Percent{ 100 }));

    actuation->Apply(5.0f, -5.0f);
}

TEST_F(MotionActuationImplTest, zero_effort_holds_both_half_bridges_low)
{
    EXPECT_CALL(left, Start(hal::Percent{ 0 }, hal::Percent{ 0 }));
    EXPECT_CALL(right, Start(hal::Percent{ 0 }, hal::Percent{ 0 }));

    actuation->Apply(0.0f, 0.0f);
}

TEST_F(MotionActuationImplTest, coast_releases_both_bridges)
{
    EXPECT_CALL(left, Stop());
    EXPECT_CALL(right, Stop());

    actuation->Disable(motion::DisableState::coast);
}

TEST_F(MotionActuationImplTest, brake_shorts_both_motors)
{
    EXPECT_CALL(left, Start(hal::Percent{ 0 }, hal::Percent{ 0 }));
    EXPECT_CALL(right, Start(hal::Percent{ 0 }, hal::Percent{ 0 }));

    actuation->Disable(motion::DisableState::brake);
}

TEST_F(MotionActuationImplTest, driver_fault_coasts_and_latches)
{
    EXPECT_CALL(left, Stop());
    EXPECT_CALL(right, Stop());

    onFault();

    EXPECT_EQ(motion::FaultCause::driverFault, actuation->Fault());
}

TEST_F(MotionActuationImplTest, fault_stays_latched_and_ignores_effort_until_cleared)
{
    EXPECT_CALL(left, Stop());
    EXPECT_CALL(right, Stop());
    onFault();

    // No Start() expectation: a strict mock fails the test if effort reaches a bridge.
    actuation->Apply(0.5f, 0.5f);
    EXPECT_EQ(motion::FaultCause::driverFault, actuation->Fault());

    actuation->ClearFault();
    EXPECT_EQ(motion::FaultCause::none, actuation->Fault());

    EXPECT_CALL(left, Start(hal::Percent{ 50 }, hal::Percent{ 0 }));
    EXPECT_CALL(right, Start(hal::Percent{ 50 }, hal::Percent{ 0 }));
    actuation->Apply(0.5f, 0.5f);
}

TEST_F(MotionActuationImplConfigTest, switching_frequency_is_configurable)
{
    motion::MotionActuationImpl::Config config;
    config.switchingFrequency = hal::Hertz{ 20000 };

    EXPECT_CALL(left, SetBaseFrequency(hal::Hertz{ 20000 }));
    EXPECT_CALL(right, SetBaseFrequency(hal::Hertz{ 20000 }));
    EXPECT_CALL(left, Stop());
    EXPECT_CALL(right, Stop());
    EXPECT_CALL(faultPin, EnableInterrupt(testing::_, hal::InterruptTrigger::fallingEdge, testing::_));

    actuation.emplace(left, right, faultPin, config);
}

TEST_F(MotionActuationImplConfigTest, an_active_high_fault_line_triggers_on_the_rising_edge)
{
    motion::MotionActuationImpl::Config config;
    config.faultActiveHigh = true;

    EXPECT_CALL(left, SetBaseFrequency(testing::_));
    EXPECT_CALL(right, SetBaseFrequency(testing::_));
    EXPECT_CALL(left, Stop());
    EXPECT_CALL(right, Stop());
    EXPECT_CALL(faultPin, EnableInterrupt(testing::_, hal::InterruptTrigger::risingEdge, testing::_));

    actuation.emplace(left, right, faultPin, config);
}
