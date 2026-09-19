#pragma once

#include "hal/synchronous_interfaces/SynchronousPwm.hpp"

namespace platform
{
    struct SignMagnitude
    {
        bool secondInputHigh;
        hal::Percent dutyCycle;
    };

    SignMagnitude AsSignMagnitude(hal::Percent input1, hal::Percent input2);
}
