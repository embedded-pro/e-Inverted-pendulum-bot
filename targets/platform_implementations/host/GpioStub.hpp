#ifndef TARGETS_PLATFORM_IMPLEMENTATIONS_HOST_GPIO_STUB_HPP
#define TARGETS_PLATFORM_IMPLEMENTATIONS_HOST_GPIO_STUB_HPP

#include "hal/interfaces/Gpio.hpp"

namespace application
{
    class GpioStub final
        : public hal::GpioPin
    {
    public:
        bool Get() const override
        {
            return false;
        }

        void Set(bool) override
        {}

        bool GetOutputLatch() const override
        {
            return false;
        }

        void SetAsInput() override
        {}

        bool IsInput() const override
        {
            return false;
        }

        void Config(hal::PinConfigType) override
        {}

        void Config(hal::PinConfigType, bool) override
        {}

        void ResetConfig() override
        {}

        void EnableInterrupt(const infra::Function<void()>&, hal::InterruptTrigger, hal::InterruptType) override
        {}

        void DisableInterrupt() override
        {}
    };
}

#endif
