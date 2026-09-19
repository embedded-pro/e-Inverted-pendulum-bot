#include "targets/platform_implementations/host/PlatformImpl.hpp"

namespace application
{
    hal::GpioPin& PlatformImpl::StatusLed()
    {
        return led;
    }

    hal::SerialCommunication& PlatformImpl::Communication()
    {
        return loopback.Client();
    }

    services::Tracer& PlatformImpl::Tracer()
    {
        return tracer;
    }

    platform::MotorDriver& PlatformImpl::Motors()
    {
        return motors;
    }

    void PlatformImpl::Run()
    {
        eventDispatcher.Run();
    }
}
