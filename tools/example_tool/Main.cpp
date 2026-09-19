#include "core/example_component/implementations/AccumulatorImpl.hpp"
#include <cstdio>
#include <cstdlib>

int main(int argc, const char* const argv[])
{
    example::AccumulatorImpl accumulator;

    for (int i = 1; i != argc; ++i)
        accumulator.Add(static_cast<int32_t>(std::atoi(argv[i])));

    std::printf("total = %d\n", accumulator.Total());

    return 0;
}
