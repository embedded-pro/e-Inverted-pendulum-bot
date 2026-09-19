#pragma once

#include "core/platform_abstraction/WheelEncoders.hpp"
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/synchronous_stm32fxxx/SynchronousQuadratureEncoderStm.hpp"

namespace application
{
    // Both wheels decoded in hardware, one per timer, because TIM1 and TIM2 are the
    // only instances on this part with an encoder mode and the motors no longer need
    // either of them.
    //
    // The wheels are mirrored, so one is configured with an inverted phase to make
    // forward robot motion count up on both.
    class WheelEncodersStm final
        : public platform::WheelEncoders
    {
    public:
        WheelEncodersStm();

        hal::SynchronousQuadratureEncoder& Left() override;
        hal::SynchronousQuadratureEncoder& Right() override;

    private:
        static hal::SynchronousQuadratureEncoderStm::Config EncoderConfig(bool mirrored);

        hal::GpioPinStm leftPhaseA{ hal::Port::A, 8 }; // TIM1_CH1
        hal::GpioPinStm leftPhaseB{ hal::Port::A, 9 }; // TIM1_CH2
        hal::GpioPinStm leftIndex{ hal::Port::C, 2 };

        hal::GpioPinStm rightPhaseA{ hal::Port::A, 0 }; // TIM2_CH1
        hal::GpioPinStm rightPhaseB{ hal::Port::A, 1 }; // TIM2_CH2
        hal::GpioPinStm rightIndex{ hal::Port::C, 3 };

        hal::SynchronousQuadratureEncoderStm left;
        hal::SynchronousQuadratureEncoderStm right;
    };
}
