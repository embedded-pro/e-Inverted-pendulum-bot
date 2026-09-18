#include "targets/platform_implementations/st/PlatformImpl.hpp"

// Referenced by the hal_st clock configuration. The NUCLEO-WB55RG has a 32 MHz
// HSE; update this when adding a board preset with a different crystal.
unsigned int hse_value = 32'000'000;

namespace application
{
    namespace
    {
        hal::PwmStmBase::Config MotorPwmConfig()
        {
            hal::PwmStmBase::Config config;

            // Centre-aligned places the pulse symmetrically in the period, which is
            // what keeps current ripple low through a brushed motor.
            config.alignment = hal::PwmStmBase::Alignment::centerAlignedBothCounting;

            hal::PwmStmBase::DeadTime deadTime;
            deadTime.duration = std::chrono::nanoseconds{ 500 };
            config.deadTime = deadTime;

            // The driver's fault line reaches every motor timer's break input, so a
            // fault releases the bridges in hardware without firmware cooperation.
            hal::PwmStmBase::BreakInput breakInput;
            breakInput.activeHigh = false;
            config.breakInput = breakInput;

            return config;
        }
    }

    PlatformImpl::SingleTimerMotorBridge::SingleTimerMotorBridge(uint8_t timerOneBasedIndex, infra::MemoryRange<const hal::PwmStmBase::ChannelConfig> channels, hal::GpioPinStm& breakPin, const hal::PwmStmBase::Config& config)
        : pwm(timerOneBasedIndex, channels, breakPin, config)
    {}

    void PlatformImpl::SingleTimerMotorBridge::SetBaseFrequency(hal::Hertz baseFrequency)
    {
        pwm.SetBaseFrequency(baseFrequency);
    }

    void PlatformImpl::SingleTimerMotorBridge::Start(hal::Percent halfBridgeA, hal::Percent halfBridgeB)
    {
        pwm.Start(halfBridgeA, halfBridgeB);
    }

    void PlatformImpl::SingleTimerMotorBridge::Stop()
    {
        pwm.Stop();
    }

    PlatformImpl::SplitTimerMotorBridge::SplitTimerMotorBridge(uint8_t timerA, infra::MemoryRange<const hal::PwmStmBase::ChannelConfig> channelsA, hal::GpioPinStm& breakPinA,
        uint8_t timerB, infra::MemoryRange<const hal::PwmStmBase::ChannelConfig> channelsB, hal::GpioPinStm& breakPinB,
        const hal::PwmStmBase::Config& config)
        : pwmA(timerA, channelsA, breakPinA, config)
        , pwmB(timerB, channelsB, breakPinB, config)
    {}

    void PlatformImpl::SplitTimerMotorBridge::SetBaseFrequency(hal::Hertz baseFrequency)
    {
        pwmA.SetBaseFrequency(baseFrequency);
        pwmB.SetBaseFrequency(baseFrequency);
    }

    void PlatformImpl::SplitTimerMotorBridge::Start(hal::Percent halfBridgeA, hal::Percent halfBridgeB)
    {
        pwmA.Start(halfBridgeA);
        pwmB.Start(halfBridgeB);
    }

    void PlatformImpl::SplitTimerMotorBridge::Stop()
    {
        pwmA.Stop();
        pwmB.Stop();
    }

    PlatformImpl::PlatformImpl()
        : leftChannels{ { { 1, leftHighA, leftLowA, false, false, false, false },
              { 2, leftHighB, leftLowB, false, false, false, false } } }
        , rightChannelsA{ { { 1, rightHighA, rightLowA, false, false, false, false } } }
        , rightChannelsB{ { { 1, rightHighB, rightLowB, false, false, false, false } } }
        , leftMotorBridge(1, leftChannels, leftBreak, MotorPwmConfig())
        , rightMotorBridge(16, rightChannelsA, rightBreakA, 17, rightChannelsB, rightBreakB, MotorPwmConfig())
    {}

    hal::GpioPin& PlatformImpl::StatusLed()
    {
        return statusLed;
    }

    hal::SerialCommunication& PlatformImpl::Communication()
    {
        return communication;
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
        return spiWithChipSelect;
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
        return backEmf;
    }

    void PlatformImpl::Run()
    {
        eventInfrastructure.Run();
    }
}
