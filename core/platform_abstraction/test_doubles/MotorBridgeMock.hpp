#ifndef CORE_PLATFORM_ABSTRACTION_TEST_DOUBLES_MOTOR_BRIDGE_MOCK_HPP
#define CORE_PLATFORM_ABSTRACTION_TEST_DOUBLES_MOTOR_BRIDGE_MOCK_HPP

#include "core/platform_abstraction/MotorBridge.hpp"
#include "gmock/gmock.h"

namespace platform
{
    class MotorBridgeMock
        : public MotorBridge
    {
    public:
        virtual ~MotorBridgeMock() = default;

        MOCK_METHOD(void, SetBaseFrequency, (hal::Hertz baseFrequency), (override));
        MOCK_METHOD(void, Start, (hal::Percent input1, hal::Percent input2), (override));
        MOCK_METHOD(void, Stop, (), (override));
    };
}

#endif
