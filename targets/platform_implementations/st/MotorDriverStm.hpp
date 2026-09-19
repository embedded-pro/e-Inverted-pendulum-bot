#pragma once

#include "core/platform_abstraction/MotorDriver.hpp"
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "targets/platform_implementations/st/MotorBridgeStm.hpp"

namespace application
{
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

        hal::GpioPinStm leftPwm{ hal::Port::B, 8 };
        hal::GpioPinStm leftDirection{ hal::Port::C, 0 };
        hal::GpioPinStm leftBreak{ hal::Port::B, 5 };

        hal::GpioPinStm rightPwm{ hal::Port::B, 9 };
        hal::GpioPinStm rightDirection{ hal::Port::C, 1 };
        hal::GpioPinStm rightBreak{ hal::Port::B, 4 };

        hal::GpioPinStm faultPin{ hal::Port::C, 4 };

        MotorBridgeStm left;
        MotorBridgeStm right;
    };
}
