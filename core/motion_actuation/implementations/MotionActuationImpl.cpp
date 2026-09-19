#include "core/motion_actuation/implementations/MotionActuationImpl.hpp"
#include <algorithm>
#include <cmath>

namespace motion
{
    namespace
    {
        // infra::Quantity is not a literal type, so these are functions rather
        // than constexpr objects; a namespace-scope object would need dynamic
        // initialization on target.
        hal::Percent Off()
        {
            return hal::Percent{ 0 };
        }

        hal::Percent Full()
        {
            return hal::Percent{ 100 };
        }

        hal::Percent DutyOf(float magnitude)
        {
            return hal::Percent{ static_cast<uint8_t>(std::lround(std::clamp(magnitude, 0.0f, 1.0f) * 100.0f)) };
        }
    }

    MotionActuationImpl::Config::Config() = default;

    MotionActuationImpl::MotionActuationImpl(platform::MotorDriver& motors, const Config& config)
        : motors(motors)
        , config(config)
    {
        motors.Left().SetBaseFrequency(config.switchingFrequency);
        motors.Right().SetBaseFrequency(config.switchingFrequency);

        ReleaseBridges();

        motors.EnableFaultNotification([this]()
            {
                OnFault();
            });
    }

    MotionActuationImpl::~MotionActuationImpl()
    {
        motors.DisableFaultNotification();
        ReleaseBridges();
    }

    void MotionActuationImpl::Apply(float effortLeft, float effortRight)
    {
        if (fault != FaultCause::none)
            return;

        ApplyTo(motors.Left(), effortLeft);
        ApplyTo(motors.Right(), effortRight);
    }

    // Magnitude selects duty, sign selects which input carries it. Monotonic, so a
    // change in commanded effort always moves the wheel the same way.
    void MotionActuationImpl::ApplyTo(platform::MotorBridge& bridge, float effort) const
    {
        const auto clamped = std::clamp(effort, -1.0f, 1.0f);
        const auto duty = DutyOf(std::fabs(clamped));

        if (clamped >= 0.0f)
            bridge.Start(duty, Off());
        else
            bridge.Start(Off(), duty);
    }

    void MotionActuationImpl::Disable(DisableState state)
    {
        if (state == DisableState::brake)
        {
            // Both inputs high turns both low-side transistors on, shorting the
            // motor. Both inputs low would release the bridge, which is a coast.
            motors.Left().Start(Full(), Full());
            motors.Right().Start(Full(), Full());
        }
        else
        {
            ReleaseBridges();
        }
    }

    void MotionActuationImpl::ReleaseBridges() const
    {
        motors.Left().Stop();
        motors.Right().Stop();
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
        ReleaseBridges();
    }
}
