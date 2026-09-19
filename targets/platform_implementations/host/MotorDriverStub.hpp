#ifndef TARGETS_PLATFORM_IMPLEMENTATIONS_HOST_MOTOR_DRIVER_STUB_HPP
#define TARGETS_PLATFORM_IMPLEMENTATIONS_HOST_MOTOR_DRIVER_STUB_HPP

#include "core/platform_abstraction/MotorDriver.hpp"
#include "targets/platform_implementations/host/MotorBridgeStub.hpp"

namespace application
{
    class MotorDriverStub final
        : public platform::MotorDriver
    {
    public:
        platform::MotorBridge& Left() override
        {
            return left;
        }

        platform::MotorBridge& Right() override
        {
            return right;
        }

        void EnableFaultNotification(const infra::Function<void()>& onFault) override
        {
            this->onFault = onFault;
        }

        void DisableFaultNotification() override
        {
            onFault = nullptr;
        }

    private:
        MotorBridgeStub left;
        MotorBridgeStub right;
        infra::Function<void()> onFault;
    };
}

#endif
