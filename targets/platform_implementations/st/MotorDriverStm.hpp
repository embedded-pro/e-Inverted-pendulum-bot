#ifndef TARGETS_PLATFORM_IMPLEMENTATIONS_ST_MOTOR_DRIVER_STM_HPP
#define TARGETS_PLATFORM_IMPLEMENTATIONS_ST_MOTOR_DRIVER_STM_HPP

#include "core/platform_abstraction/MotorDriver.hpp"
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/synchronous_stm32fxxx/SynchronousPwmStm.hpp"
#include "targets/platform_implementations/st/MotorBridgeStm.hpp"
#include <array>

namespace application
{
    // A DRV8711 driving two brushed motors from its two full bridges. In direct
    // PWM mode it takes four logic-level inputs and generates its own gate drive
    // and dead time, so the timer needs neither complementary outputs nor a
    // dead-time generator.
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

        hal::GpioPinStm leftInput1{ hal::Port::A, 8 };   // TIM1_CH1 -> AIN1
        hal::GpioPinStm leftInput2{ hal::Port::A, 9 };   // TIM1_CH2 -> AIN2
        hal::GpioPinStm rightInput1{ hal::Port::A, 10 }; // TIM1_CH3 -> BIN1
        hal::GpioPinStm rightInput2{ hal::Port::A, 11 }; // TIM1_CH4 -> BIN2
        hal::GpioPinStm breakPin{ hal::Port::B, 12 };    // TIM1_BKIN <- nFAULT
        hal::GpioPinStm faultPin{ hal::Port::C, 4 };     // nFAULT, for the software latch

        std::array<hal::PwmStmBase::ChannelConfig, 4> channels;
        hal::SynchronousPwmStm pwm;
        std::array<hal::Percent, 4> dutyCycles{ { hal::Percent{ 0 }, hal::Percent{ 0 }, hal::Percent{ 0 }, hal::Percent{ 0 } } };

        MotorBridgeStm left;
        MotorBridgeStm right;
    };
}

#endif
