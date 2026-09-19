#ifndef CORE_PLATFORM_ABSTRACTION_MOTOR_BRIDGE_HPP
#define CORE_PLATFORM_ABSTRACTION_MOTOR_BRIDGE_HPP

#include "hal/synchronous_interfaces/SynchronousPwm.hpp"

namespace platform
{
    class MotorBridge
    {
    public:
        MotorBridge() = default;
        MotorBridge(const MotorBridge& other) = delete;
        MotorBridge& operator=(const MotorBridge& other) = delete;

        virtual void SetBaseFrequency(hal::Hertz baseFrequency) = 0;
        virtual void Start(hal::Percent input1, hal::Percent input2) = 0;
        virtual void Stop() = 0;

    protected:
        ~MotorBridge() = default;
    };
}

#endif
