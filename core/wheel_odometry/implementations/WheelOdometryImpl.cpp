#include "core/wheel_odometry/implementations/WheelOdometryImpl.hpp"
#include "infra/util/ReallyAssert.hpp"
#include <numbers>

namespace
{
    std::chrono::microseconds VerifiedSamplePeriod(std::chrono::microseconds samplePeriod)
    {
        really_assert(samplePeriod.count() > 0);
        return samplePeriod;
    }
}

namespace odometry
{
    WheelOdometryImpl::Config::Config() = default;

    WheelOdometryImpl::WheelOdometryImpl(platform::WheelEncoders& encoders, const Config& config)
        : encoders(encoders)
        , config(config)
        , sampleTimer{ VerifiedSamplePeriod(config.samplePeriod), [this]()
            {
                Sample();
            } }
    {
        really_assert(config.wheelRadius > 0.0f);
        really_assert(config.trackWidth > 0.0f);
        really_assert(config.gearRatio > 0.0f);
    }

    int32_t WheelOdometryImpl::ShortestDelta(uint32_t previous, uint32_t current, uint32_t resolution)
    {
        const auto span = static_cast<int32_t>(resolution);
        auto delta = static_cast<int32_t>(current) - static_cast<int32_t>(previous);

        if (delta > span / 2)
            delta -= span;
        else if (delta < -span / 2)
            delta += span;

        return delta;
    }

    void WheelOdometryImpl::SampleWheel(Wheel& wheel, hal::SynchronousQuadratureEncoder& encoder) const
    {
        const auto resolution = encoder.Resolution();
        const auto raw = encoder.Position();

        really_assert(resolution >= 2);
        really_assert(raw < resolution);

        if (!wheel.sampled)
        {
            wheel.previousRaw = raw;
            wheel.sampled = true;
            return;
        }

        const auto delta = ShortestDelta(wheel.previousRaw, raw, resolution);
        wheel.previousRaw = raw;

        const auto seconds = std::chrono::duration<float>(config.samplePeriod).count();
        const auto countsPerWheelRevolution = static_cast<float>(resolution) * config.gearRatio;

        wheel.motion.position += delta;
        wheel.motion.angularVelocity = 2.0f * std::numbers::pi_v<float> * static_cast<float>(delta) / (countsPerWheelRevolution * seconds);
    }

    void WheelOdometryImpl::Sample()
    {
        SampleWheel(left, encoders.Left());
        SampleWheel(right, encoders.Right());
    }

    WheelMotion WheelOdometryImpl::Left() const
    {
        return left.motion;
    }

    WheelMotion WheelOdometryImpl::Right() const
    {
        return right.motion;
    }

    ChassisMotion WheelOdometryImpl::Chassis() const
    {
        ChassisMotion chassis;

        chassis.forwardVelocity = config.wheelRadius * (left.motion.angularVelocity + right.motion.angularVelocity) / 2.0f;
        chassis.yawRate = config.wheelRadius * (right.motion.angularVelocity - left.motion.angularVelocity) / config.trackWidth;

        return chassis;
    }
}
