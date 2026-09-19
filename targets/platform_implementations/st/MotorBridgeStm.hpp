#pragma once

#include "core/platform_abstraction/MotorBridge.hpp"
#include "hal/interfaces/Gpio.hpp"
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/synchronous_stm32fxxx/SynchronousPwmStm.hpp"
#include <array>

namespace application
{
    // One motor driven sign-magnitude: a single PWM line into the driver's first
    // input, and the second input held as a direction level. One timer channel per
    // motor instead of two, which is what leaves TIM1 and TIM2 free to decode both
    // wheel encoders in hardware.
    //
    // The direction decides which decay mode the bridge falls into. Forward toggles
    // between driven and released; reverse toggles between driven and shorted, so
    // reverse recirculates through the low side and forward does not. Commanded
    // magnitudes match, current ripple does not.
    class MotorBridgeStm final
        : public platform::MotorBridge
    {
    public:
        MotorBridgeStm(uint8_t timerOneBasedIndex, hal::GpioPinStm& pwmPin, hal::GpioPinStm& breakPin, hal::GpioPin& directionPin, const hal::PwmStmBase::Config& config);

        void SetBaseFrequency(hal::Hertz baseFrequency) override;
        void Start(hal::Percent input1, hal::Percent input2) override;
        void Stop() override;

    private:
        std::array<hal::PwmStmBase::ChannelConfig, 1> channels;
        hal::SynchronousPwmStm pwm;
        hal::OutputPin direction;
    };
}
