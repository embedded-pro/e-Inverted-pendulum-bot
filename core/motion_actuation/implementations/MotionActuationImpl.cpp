#include "core/motion_actuation/implementations/MotionActuationImpl.hpp"
#include <algorithm>
#include <cmath>

namespace motion
{
    namespace
    {
        hal::Percent DutyOf(float magnitude)
        {
            return hal::Percent{ static_cast<uint8_t>(std::lround(std::clamp(magnitude, 0.0f, 1.0f) * 100.0f)) };
        }
    }

    MotionActuationImpl::Config::Config() = default;

    MotionActuationImpl::MotionActuationImpl(platform::MotorBridge& left, platform::MotorBridge& right, hal::GpioPin& faultPin, const Config& config)
        : left(left)
        , right(right)
        , faultPin(faultPin)
        , config(config)
    {
        left.SetBaseFrequency(config.switchingFrequency);
        right.SetBaseFrequency(config.switchingFrequency);

        Coast();

        faultPin.EnableInterrupt([this]()
            {
                OnFault();
            },
            config.faultActiveHigh ? hal::InterruptTrigger::risingEdge : hal::InterruptTrigger::fallingEdge);
    }

    MotionActuationImpl::~MotionActuationImpl()
    {
        faultPin.DisableInterrupt();
        Coast();
    }

    void MotionActuationImpl::Apply(float effortLeft, float effortRight)
    {
        if (fault != FaultCause::none)
            return;

        ApplyTo(left, effortLeft);
        ApplyTo(right, effortRight);
    }

    // Magnitude selects duty, sign selects which half-bridge carries it. Monotonic,
    // so a change in commanded effort always moves the wheel the same way.
    void MotionActuationImpl::ApplyTo(platform::MotorBridge& bridge, float effort) const
    {
        const auto clamped = std::clamp(effort, -1.0f, 1.0f);
        const auto duty = DutyOf(std::fabs(clamped));

        if (clamped >= 0.0f)
            bridge.Start(duty, hal::Percent{ 0 });
        else
            bridge.Start(hal::Percent{ 0 }, duty);
    }

    void MotionActuationImpl::Disable(DisableState state)
    {
        if (state == DisableState::brake)
        {
            left.Start(hal::Percent{ 0 }, hal::Percent{ 0 });
            right.Start(hal::Percent{ 0 }, hal::Percent{ 0 });
        }
        else
        {
            Coast();
        }
    }

    void MotionActuationImpl::Coast() const
    {
        left.Stop();
        right.Stop();
    }

    FaultCause MotionActuationImpl::Fault() const
    {
        return fault;
    }

    void MotionActuationImpl::ClearFault()
    {
        fault = FaultCause::none;
    }

    void MotionActuationImpl::OnFault()
    {
        fault = FaultCause::driverFault;
        Coast();
    }
}
