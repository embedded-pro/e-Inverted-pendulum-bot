#pragma once

#include "core/example_component/implementations/AccumulatorImpl.hpp"

namespace integration
{
    // Fixture stored in the cucumber `context` and shared across steps in a
    // scenario. A real suite would assemble the platform + application here so
    // steps drive the full system under test.
    struct AccumulatorFixture
    {
        example::AccumulatorImpl accumulator;
    };
}
