#include "targets/platform_implementations/st/MotorDriverStm.hpp"

namespace application
{
    hal::PwmStmBase::Config MotorDriverStm::PwmConfig()
    {
        hal::PwmStmBase::Config config;

        config.alignment = hal::PwmStmBase::Alignment::centerAlignedBothCounting;

        hal::PwmStmBase::BreakInput breakInput;
        breakInput.activeHigh = false;
        config.breakInput = breakInput;

        return config;
    }

    MotorDriverStm::MotorDriverStm()
        : left(16, leftPwm, leftBreak, leftDirection, PwmConfig())
        , right(17, rightPwm, rightBreak, rightDirection, PwmConfig())
    {}

    platform::MotorBridge& MotorDriverStm::Left()
    {
        return left;
    }

    platform::MotorBridge& MotorDriverStm::Right()
    {
        return right;
    }

    void MotorDriverStm::EnableFaultNotification(const infra::Function<void()>& onFault)
    {
        faultPin.EnableInterrupt(onFault, hal::InterruptTrigger::fallingEdge);
    }

    void MotorDriverStm::DisableFaultNotification()
    {
        faultPin.DisableInterrupt();
    }
}
