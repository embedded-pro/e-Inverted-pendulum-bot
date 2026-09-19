#include "targets/platform_implementations/st/MotorDriverStm.hpp"

namespace application
{
    hal::PwmStmBase::Config MotorDriverStm::PwmConfig()
    {
        hal::PwmStmBase::Config config;

        // Centre-aligned keeps current ripple low through a brushed motor.
        config.alignment = hal::PwmStmBase::Alignment::centerAlignedBothCounting;

        // nFAULT reaches the break input, so a fault releases all four outputs in
        // hardware without firmware cooperation.
        hal::PwmStmBase::BreakInput breakInput;
        breakInput.activeHigh = false;
        config.breakInput = breakInput;

        return config;
    }

    MotorDriverStm::MotorDriverStm()
        : channels{ { { 1, leftInput1 },
              { 2, leftInput2 },
              { 3, rightInput1 },
              { 4, rightInput2 } } }
        , pwm(1, channels, breakPin, PwmConfig())
        , left(pwm, dutyCycles, 0)
        , right(pwm, dutyCycles, 2)
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
