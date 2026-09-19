#pragma once

#include "core/platform_abstraction/MotorDriver.hpp"
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "targets/platform_implementations/st/MotorBridgeStm.hpp"

namespace application
{
    // A DRV8711 driving two brushed motors from its two full bridges. In direct PWM
    // mode it takes two logic-level inputs per bridge and generates its own gate
    // drive and dead time, so the timer needs neither complementary outputs nor a
    // dead-time generator.
    //
    // Each motor takes one channel of a single-channel timer, leaving the two
    // encoder-capable timers for the wheels. The cost is that the two motors no
    // longer share an update event, so their switching edges are not phase-locked.
    class MotorDriverStm final
        : public platform::MotorDriver
    {
    public:
        MotorDriverStm();

        platform::MotorBridge& Left() override;
        platform::MotorBridge& Right() override;
        void EnableFaultNotification(const infra::Function<void()>& onFault) override;
        void DisableFaultNotification() override;

    private:
        static hal::PwmStmBase::Config PwmConfig();

        hal::GpioPinStm leftPwm{ hal::Port::B, 8 };        // TIM16_CH1 -> AIN1
        hal::GpioPinStm leftDirection{ hal::Port::C, 0 };  // AIN2
        hal::GpioPinStm leftBreak{ hal::Port::B, 5 };      // TIM16_BKIN <- nFAULT

        hal::GpioPinStm rightPwm{ hal::Port::B, 9 };       // TIM17_CH1 -> BIN1
        hal::GpioPinStm rightDirection{ hal::Port::C, 1 }; // BIN2
        hal::GpioPinStm rightBreak{ hal::Port::B, 4 };     // TIM17_BKIN <- nFAULT

        hal::GpioPinStm faultPin{ hal::Port::C, 4 };       // nFAULT, for the software latch

        MotorBridgeStm left;
        MotorBridgeStm right;
    };
}
