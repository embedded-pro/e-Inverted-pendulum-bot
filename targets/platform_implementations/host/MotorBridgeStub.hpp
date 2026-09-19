#ifndef TARGETS_PLATFORM_IMPLEMENTATIONS_HOST_MOTOR_BRIDGE_STUB_HPP
#define TARGETS_PLATFORM_IMPLEMENTATIONS_HOST_MOTOR_BRIDGE_STUB_HPP

#include "core/platform_abstraction/MotorBridge.hpp"

namespace application
{
    // Accepts every command and drives nothing, so the whole control stack is
    // constructible on the host. A simulated plant replaces this later.
    class MotorBridgeStub final
        : public platform::MotorBridge
    {
    public:
        void SetBaseFrequency(hal::Hertz) override
        {}

        void Start(hal::Percent, hal::Percent) override
        {}

        void Stop() override
        {}
    };
}

#endif
