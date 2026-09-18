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

    platform::MotorBridge& PlatformImpl::LeftMotorBridge()
    {
        return leftMotorBridge;
    }

    platform::MotorBridge& PlatformImpl::RightMotorBridge()
    {
        return rightMotorBridge;
    }

    hal::SpiMaster& PlatformImpl::MotorDriverConfiguration()
    {
        return motorDriverSpi;
    }

    hal::GpioPin& PlatformImpl::MotorDriverFault()
    {
        return motorDriverFault;
    }

    hal::GpioPin& PlatformImpl::MotorDriverStall()
    {
        return motorDriverStall;
    }

    hal::AnalogToDigitalPin<infra::MilliVolt, uint32_t>& PlatformImpl::MotorDriverBackEmf()
    {
        return motorDriverBackEmf;
    }

    void PlatformImpl::Run()
    {
        eventDispatcher.Run();
    }
}
