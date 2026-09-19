#ifndef CORE_PLATFORM_ABSTRACTION_TEST_DOUBLES_MOTOR_DRIVER_MOCK_HPP
#define CORE_PLATFORM_ABSTRACTION_TEST_DOUBLES_MOTOR_DRIVER_MOCK_HPP

#include "core/platform_abstraction/MotorDriver.hpp"
#include "gmock/gmock.h"

namespace platform
{
    class MotorDriverMock
        : public MotorDriver
    {
    public:
        virtual ~MotorDriverMock() = default;

        MOCK_METHOD(MotorBridge&, Left, (), (override));
        MOCK_METHOD(MotorBridge&, Right, (), (override));
        MOCK_METHOD(void, EnableFaultNotification, (const infra::Function<void()>& onFault), (override));
        MOCK_METHOD(void, DisableFaultNotification, (), (override));
    };
}

#endif
