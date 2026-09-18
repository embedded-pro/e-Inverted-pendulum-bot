#ifndef CORE_MOTION_ACTUATION_INTERFACES_MOTION_ACTUATION_HPP
#define CORE_MOTION_ACTUATION_INTERFACES_MOTION_ACTUATION_HPP

#include <cstdint>

namespace motion
{
    // How the drive is de-energised. Safety-initiated disables always coast: a
    // falling robot that brakes plants its wheels and converts a topple into a
    // harder impact, and braking still drives current through the bridges at the
    // moment a fault is suspected.
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

        // Normalised and signed, -1.0 to 1.0; the sign selects direction. Values
        // outside the range are clamped. Ignored while faulted.
        virtual void Apply(float effortLeft, float effortRight) = 0;

        virtual void Disable(DisableState state) = 0;

        // Latched on assertion, even if the driver deasserts immediately.
        virtual FaultCause Fault() const = 0;
        virtual void ClearFault() = 0;

    protected:
        ~MotionActuation() = default;
    };
}

#endif
