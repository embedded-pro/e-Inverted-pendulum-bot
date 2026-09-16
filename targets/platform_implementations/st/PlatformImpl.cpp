#include "targets/platform_implementations/st/PlatformImpl.hpp"

// Referenced by the hal_st clock configuration. The NUCLEO-WB55RG has a 32 MHz
// HSE; update this when adding a board preset with a different crystal.
unsigned int hse_value = 32'000'000;

namespace application
{
    hal::GpioPin& PlatformImpl::StatusLed()
    {
        return statusLed;
    }

    hal::SerialCommunication& PlatformImpl::Communication()
    {
        return uart;
    }

    services::Tracer& PlatformImpl::Tracer()
    {
        return tracer;
    }

    void PlatformImpl::Run()
    {
        eventInfrastructure.Run();
    }
}
