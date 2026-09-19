#pragma once

#include "hal/synchronous_interfaces/SynchronousQuadratureEncoder.hpp"

namespace application
{
    // A stationary wheel, so the whole stack is constructible on the host. A
    // simulated plant replaces this later.
    class WheelEncoderStub final
        : public hal::SynchronousQuadratureEncoder
    {
    public:
        uint32_t Position() override
        {
            return 0;
        }

        uint32_t Resolution() override
        {
            return resolution;
        }

        MotionDirection Direction() override
        {
            return MotionDirection::forward;
        }

        uint32_t Speed() override
        {
            return 0;
        }

    private:
        static constexpr uint32_t resolution = 4096;
    };
}
