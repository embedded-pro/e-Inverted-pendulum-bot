#pragma once

#include "core/platform_abstraction/WheelEncoders.hpp"
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/synchronous_stm32fxxx/SynchronousQuadratureEncoderStm.hpp"
#include <chrono>

namespace application
{
    class WheelEncodersStm final
        : public platform::WheelEncoders
    {
    public:
        WheelEncodersStm();

        hal::SynchronousQuadratureEncoder& Left() override;
        hal::SynchronousQuadratureEncoder& Right() override;

    private:
        static constexpr std::chrono::microseconds speedSamplePeriod{ 2000 };

        static hal::SynchronousQuadratureEncoderStm::Config EncoderConfig(bool mirrored);

        hal::GpioPinStm leftPhaseA{ hal::Port::A, 8 };
        hal::GpioPinStm leftPhaseB{ hal::Port::A, 9 };
        hal::GpioPinStm leftIndex{ hal::Port::C, 2 };

        hal::GpioPinStm rightPhaseA{ hal::Port::A, 0 };
        hal::GpioPinStm rightPhaseB{ hal::Port::A, 1 };
        hal::GpioPinStm rightIndex{ hal::Port::C, 3 };

        hal::SynchronousQuadratureEncoderStm left;
        hal::SynchronousQuadratureEncoderStm right;
    };
}
