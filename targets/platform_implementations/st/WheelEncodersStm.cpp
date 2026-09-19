#include "targets/platform_implementations/st/WheelEncodersStm.hpp"

namespace application
{
    hal::SynchronousQuadratureEncoderStm::Config WheelEncodersStm::EncoderConfig(bool mirrored)
    {
        hal::SynchronousQuadratureEncoderStm::Config config;

        config.decodeMode = hal::SynchronousQuadratureEncoderStm::Config::DecodeMode::x4OnBothPhases;
        config.invertPhaseA = mirrored;

        return config;
    }

    WheelEncodersStm::WheelEncodersStm()
        : left(1, leftPhaseA, leftPhaseB, leftIndex, EncoderConfig(false))
        , right(2, rightPhaseA, rightPhaseB, rightIndex, EncoderConfig(true))
    {}

    hal::SynchronousQuadratureEncoder& WheelEncodersStm::Left()
    {
        return left;
    }

    hal::SynchronousQuadratureEncoder& WheelEncodersStm::Right()
    {
        return right;
    }
}
