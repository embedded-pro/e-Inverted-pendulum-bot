#include PLATFORM_IMPL_HEADER
#include "core/blinky_cli/BlinkyCli.hpp"
#include "core/motion_actuation/implementations/MotionActuationImpl.hpp"

// Platform-agnostic entry point.
//
// PLATFORM_IMPL_HEADER is set per platform by targets/CMakeLists.txt and resolves to
// the selected board's PlatformImpl (host / st). The application is constructed
// against the abstract platform::Platform interface, then the platform's event loop
// takes over. This single Main.cpp is reused unchanged across every target.
int main()
{
    static application::PlatformImpl platform;
    static motion::MotionActuationImpl motionActuation{ platform.LeftMotorBridge(), platform.RightMotorBridge(), platform.MotorDriverFault() };
    static application::BlinkyCli blinkyCli{ platform, motionActuation };

    platform.Run();

#if defined(__GNUC__) || defined(__clang__)
    __builtin_unreachable();
#endif
}
