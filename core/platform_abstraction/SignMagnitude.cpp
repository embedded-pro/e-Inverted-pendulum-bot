#include "core/platform_abstraction/SignMagnitude.hpp"
#include "infra/util/ReallyAssert.hpp"

namespace platform
{
    namespace
    {
        hal::Percent Off()
        {
            return hal::Percent{ 0 };
        }

        hal::Percent Full()
        {
            return hal::Percent{ 100 };
        }
    }

    SignMagnitude AsSignMagnitude(hal::Percent input1, hal::Percent input2)
    {
        really_assert(input1 == Off() || input2 == Off() || (input1 == Full() && input2 == Full()));

        if (input1 == Full() && input2 == Full())
            return { true, Full() };

        if (input2 == Off())
            return { false, input1 };

        return { true, Full() - input2 };
    }
}
