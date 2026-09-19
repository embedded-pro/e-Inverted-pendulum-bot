#include PLATFORM_IMPL_HEADER
#include "core/cli/Cli.hpp"
#include "core/motion_actuation/implementations/MotionActuationImpl.hpp"

int main()
{
    static application::PlatformImpl platform;
    static motion::MotionActuationImpl motionActuation{ platform.Motors() };
    static application::Cli cli{ platform, motionActuation };

    platform.Run();

#if defined(__GNUC__) || defined(__clang__)
    __builtin_unreachable();
#endif
}
