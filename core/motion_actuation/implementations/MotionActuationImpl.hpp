#pragma once

#include "core/motion_actuation/interfaces/MotionActuation.hpp"
#include "core/platform_abstraction/MotorDriver.hpp"

namespace motion
{
    class MotionActuationImpl final
        : public MotionActuation
    {
    public:
        struct Config
        {
            Config();

            hal::Hertz switchingFrequency{ 25000 };
        };

        explicit MotionActuationImpl(platform::MotorDriver& motors, const Config& config = Config());
        MotionActuationImpl(const MotionActuationImpl& other) = delete;
        MotionActuationImpl& operator=(const MotionActuationImpl& other) = delete;
        ~MotionActuationImpl();

        void Apply(float effortLeft, float effortRight) override;
        void Disable(DisableState state) override;
        FaultCause Fault() const override;
        void ClearFault() override;

    private:
        void ApplyTo(platform::MotorBridge& bridge, float effort) const;

        void ReleaseBridges() const;

        void OnFault();

        platform::MotorDriver& motors;
        FaultCause fault{ FaultCause::none };
    };
}
