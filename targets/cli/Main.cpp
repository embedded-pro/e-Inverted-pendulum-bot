#include PLATFORM_IMPL_HEADER
#include "core/cli/Cli.hpp"
#include "core/motion_actuation/implementations/MotionActuationImpl.hpp"
#include "core/wheel_odometry/implementations/WheelOdometryImpl.hpp"

int main()
{
    static application::PlatformImpl platform;
    static motion::MotionActuationImpl motionActuation{ platform.Motors() };
    static odometry::WheelOdometryImpl wheelOdometry{ platform.Encoders() };
    static application::Cli cli{ platform, motionActuation, wheelOdometry };

    platform.Run();

#if defined(__GNUC__) || defined(__clang__)
    __builtin_unreachable();
#endif
}
