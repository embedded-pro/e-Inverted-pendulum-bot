#include "targets/platform_implementations/st/MotorBridgeStm.hpp"

namespace application
{
    namespace
    {
        hal::Percent Off()
        {
            return hal::Percent{ 0 };
        }

        hal::Percent Full()
        {
            return hal::Percent{ 100 };
        }
    }

    MotorBridgeStm::MotorBridgeStm(uint8_t timerOneBasedIndex, hal::GpioPinStm& pwmPin, hal::GpioPinStm& breakPin, hal::GpioPin& directionPin, const hal::PwmStmBase::Config& config)
        : channels{ { { 1, pwmPin } } }
        , pwm(timerOneBasedIndex, channels, breakPin, config)
        , direction(directionPin)
    {}

    void MotorBridgeStm::SetBaseFrequency(hal::Hertz baseFrequency)
    {
        pwm.SetBaseFrequency(baseFrequency);
    }

    // Both inputs arrive as duty cycles, but only the first is a timer channel here.
    // Both high is the brake; otherwise the zero input picks the direction and the
    // other carries the magnitude.
    void MotorBridgeStm::Start(hal::Percent input1, hal::Percent input2)
    {
        if (input1 == Full() && input2 == Full())
        {
            direction.Set(true);
            pwm.Start(Full());
        }
        else if (input2 == Off())
        {
            direction.Set(false);
            pwm.Start(input1);
        }
        else
        {
            // Reverse recirculates through the brake state, so the duty cycle that
            // produces the commanded magnitude is its complement.
            direction.Set(true);
            pwm.Start(Full() - input2);
        }
    }

    void MotorBridgeStm::Stop()
    {
        Start(Off(), Off());
    }
}
