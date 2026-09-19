#include "targets/platform_implementations/st/MotorBridgeStm.hpp"

namespace application
{
    MotorBridgeStm::MotorBridgeStm(hal::SynchronousPwmStm& pwm, infra::MemoryRange<hal::Percent> dutyCycles, std::size_t firstInput)
        : pwm(pwm)
        , dutyCycles(dutyCycles)
        , firstInput(firstInput)
    {}

    void MotorBridgeStm::SetBaseFrequency(hal::Hertz baseFrequency)
    {
        pwm.SetBaseFrequency(baseFrequency);
    }

    void MotorBridgeStm::Start(hal::Percent input1, hal::Percent input2)
    {
        dutyCycles[firstInput] = input1;
        dutyCycles[firstInput + 1] = input2;

        pwm.Start(dutyCycles[0], dutyCycles[1], dutyCycles[2], dutyCycles[3]);
    }

    void MotorBridgeStm::Stop()
    {
        Start(hal::Percent{ 0 }, hal::Percent{ 0 });
    }
}
