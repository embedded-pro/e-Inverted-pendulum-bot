#ifndef TARGETS_PLATFORM_IMPLEMENTATIONS_ST_MOTOR_BRIDGE_STM_HPP
#define TARGETS_PLATFORM_IMPLEMENTATIONS_ST_MOTOR_BRIDGE_STM_HPP

#include "core/platform_abstraction/MotorBridge.hpp"
#include "hal_st/synchronous_stm32fxxx/SynchronousPwmStm.hpp"
#include "infra/util/MemoryRange.hpp"

namespace application
{
    // Both motors' four driver inputs are four channels of one timer, so a change
    // on either bridge re-issues all four duty cycles from the shared range.
    //
    // Stop() drives both of this motor's inputs low, which is what releases a
    // DRV8711 bridge. Stopping the timer instead would release the other motor too.
    class MotorBridgeStm final
        : public platform::MotorBridge
    {
    public:
        MotorBridgeStm(hal::SynchronousPwmStm& pwm, infra::MemoryRange<hal::Percent> dutyCycles, std::size_t firstInput);

        void SetBaseFrequency(hal::Hertz baseFrequency) override;
        void Start(hal::Percent input1, hal::Percent input2) override;
        void Stop() override;

    private:
        hal::SynchronousPwmStm& pwm;
        infra::MemoryRange<hal::Percent> dutyCycles;
        std::size_t firstInput;
    };
}

#endif
