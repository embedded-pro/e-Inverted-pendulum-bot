#include "core/example_component/implementations/AccumulatorImpl.hpp"
#include <cstdio>

int main()
{
    example::AccumulatorImpl accumulator;
    accumulator.Add(2);
    accumulator.Add(3);

    std::printf("accumulator total = %d\n", accumulator.Total());

    return 0;
}
