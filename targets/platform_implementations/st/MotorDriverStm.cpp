#include "targets/platform_implementations/st/MotorDriverStm.hpp"

namespace application
{
    hal::PwmStmBase::Config MotorDriverStm::PwmConfig()
    {
        hal::PwmStmBase::Config config;

        // Centre-aligned keeps current ripple low through a brushed motor.
        config.alignment = hal::PwmStmBase::Alignment::centerAlignedBothCounting;

        // nFAULT reaches both timers' break inputs, so a fault releases the outputs
        // in hardware without firmware cooperation.
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
        // The DRV8711 pulls nFAULT low on assertion.
        faultPin.EnableInterrupt(onFault, hal::InterruptTrigger::fallingEdge);
    }

    void MotorDriverStm::DisableFaultNotification()
    {
        faultPin.DisableInterrupt();
    }
}
