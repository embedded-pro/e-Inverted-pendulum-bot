#pragma once

#include "core/platform_abstraction/Platform.hpp"
#include "gmock/gmock.h"

namespace platform
{
    class PlatformMock
        : public Platform
    {
    public:
        // Not final, and with a public virtual destructor: StrictMock<> derives from it
        // and tests instantiate it directly.
        virtual ~PlatformMock() = default;

        MOCK_METHOD(hal::GpioPin&, StatusLed, (), (override));
        MOCK_METHOD(hal::SerialCommunication&, Communication, (), (override));
        MOCK_METHOD(services::Tracer&, Tracer, (), (override));
        MOCK_METHOD(MotorDriver&, Motors, (), (override));
        MOCK_METHOD(WheelEncoders&, Encoders, (), (override));
        MOCK_METHOD(void, Run, (), (override));
    };
}
