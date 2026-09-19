#pragma once

#include "core/platform_abstraction/MotorBridge.hpp"

namespace application
{
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
