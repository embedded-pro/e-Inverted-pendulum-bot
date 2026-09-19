#pragma once

#include <cstdint>

namespace example
{
    class Accumulator
    {
    public:
        Accumulator() = default;
        Accumulator(const Accumulator& other) = delete;
        Accumulator& operator=(const Accumulator& other) = delete;

        virtual void Add(int32_t value) = 0;
        virtual int32_t Total() const = 0;
        virtual void Reset() = 0;

    protected:
        ~Accumulator() = default;
    };
}
