#ifndef CORE_MOTION_ACTUATION_IMPLEMENTATIONS_MOTION_ACTUATION_IMPL_HPP
#define CORE_MOTION_ACTUATION_IMPLEMENTATIONS_MOTION_ACTUATION_IMPL_HPP

#include "core/motion_actuation/interfaces/MotionActuation.hpp"
#include "core/platform_abstraction/MotorBridge.hpp"
#include "hal/interfaces/Gpio.hpp"

namespace motion
{
    class MotionActuationImpl final
        : public MotionActuation
    {
    public:
        struct Config
        {
            // Defaulted out of line: the constructor below takes a Config()
            // default argument, and defining this one here would need the
            // initializers below before the enclosing class is complete.
            // Not constexpr either: infra::Quantity has no constexpr constructor.
            Config();

            // Above the audible band and compatible with the motor's electrical
            // time constant.
            hal::Hertz switchingFrequency{ 25000 };

            // The driver pulls its fault line low on assertion.
            bool faultActiveHigh{ false };
        };

        MotionActuationImpl(platform::MotorBridge& left, platform::MotorBridge& right, hal::GpioPin& faultPin, const Config& config = Config());
        MotionActuationImpl(const MotionActuationImpl& other) = delete;
        MotionActuationImpl& operator=(const MotionActuationImpl& other) = delete;
        ~MotionActuationImpl();

        void Apply(float effortLeft, float effortRight) override;
        void Disable(DisableState state) override;
        FaultCause Fault() const override;
        void ClearFault() override;

    private:
        void ApplyTo(platform::MotorBridge& bridge, float effort) const;

        // Non-virtual: reached from the constructor and destructor, where a
        // virtual Disable() would not dispatch to an override anyway.
        void Coast() const;

        void OnFault();

        platform::MotorBridge& left;
        platform::MotorBridge& right;
        hal::GpioPin& faultPin;
        Config config;
        FaultCause fault{ FaultCause::none };
    };
}

#endif
