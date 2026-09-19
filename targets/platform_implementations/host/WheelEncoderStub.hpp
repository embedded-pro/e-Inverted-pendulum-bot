#pragma once

#include "hal/synchronous_interfaces/SynchronousQuadratureEncoder.hpp"

namespace application
{
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
