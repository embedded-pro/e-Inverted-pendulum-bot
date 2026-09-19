#pragma once

#include "core/platform_abstraction/MotorBridge.hpp"
#include "infra/util/Function.hpp"

namespace platform
{
    class MotorDriver
    {
    public:
        MotorDriver() = default;
        MotorDriver(const MotorDriver& other) = delete;
        MotorDriver& operator=(const MotorDriver& other) = delete;

        virtual MotorBridge& Left() = 0;
        virtual MotorBridge& Right() = 0;

        virtual void EnableFaultNotification(const infra::Function<void()>& onFault) = 0;
        virtual void DisableFaultNotification() = 0;

    protected:
        ~MotorDriver() = default;
    };
}
