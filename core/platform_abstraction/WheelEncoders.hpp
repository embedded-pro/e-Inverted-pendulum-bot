#pragma once

#include "hal/synchronous_interfaces/SynchronousQuadratureEncoder.hpp"

namespace platform
{
    class WheelEncoders
    {
    public:
        WheelEncoders() = default;
        WheelEncoders(const WheelEncoders& other) = delete;
        WheelEncoders& operator=(const WheelEncoders& other) = delete;

        virtual hal::SynchronousQuadratureEncoder& Left() = 0;
        virtual hal::SynchronousQuadratureEncoder& Right() = 0;

    protected:
        ~WheelEncoders() = default;
    };
}
