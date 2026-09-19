#ifndef CORE_MOTION_ACTUATION_INTERFACES_MOTION_ACTUATION_HPP
#define CORE_MOTION_ACTUATION_INTERFACES_MOTION_ACTUATION_HPP

#include <cstdint>

namespace motion
{
    enum class DisableState : uint8_t
    {
        coast,
        brake
    };

    enum class FaultCause : uint8_t
    {
        none,
        driverFault
    };

    class MotionActuation
    {
    public:
        MotionActuation() = default;
        MotionActuation(const MotionActuation& other) = delete;
        MotionActuation& operator=(const MotionActuation& other) = delete;

        virtual void Apply(float effortLeft, float effortRight) = 0;
        virtual void Disable(DisableState state) = 0;
        virtual FaultCause Fault() const = 0;
        virtual void ClearFault() = 0;

    protected:
        ~MotionActuation() = default;
    };
}

#endif
