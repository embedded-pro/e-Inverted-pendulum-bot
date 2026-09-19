#pragma once

#include "core/platform_abstraction/WheelEncoders.hpp"
#include "targets/platform_implementations/host/WheelEncoderStub.hpp"

namespace application
{
    class WheelEncodersStub final
        : public platform::WheelEncoders
    {
    public:
        hal::SynchronousQuadratureEncoder& Left() override
        {
            return left;
        }

        hal::SynchronousQuadratureEncoder& Right() override
        {
            return right;
        }

    private:
        WheelEncoderStub left;
        WheelEncoderStub right;
    };
}
