#ifndef TARGETS_PLATFORM_IMPLEMENTATIONS_ST_PLATFORM_IMPL_HPP
#define TARGETS_PLATFORM_IMPLEMENTATIONS_ST_PLATFORM_IMPL_HPP

#include "core/platform_abstraction/Platform.hpp"
#include "hal_st/instantiations/StmEventInfrastructure.hpp"
#include INVERTED_PENDULUM_BOT_ST_CLOCK_HEADER
#include "hal_st/stm32fxxx/AnalogToDigitalPinStm.hpp"
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/stm32fxxx/SpiMasterStm.hpp"
#include "hal_st/synchronous_stm32fxxx/SynchronousPwmStm.hpp"
#include "infra/stream/OutputStream.hpp"
#include "services/peripheral/SpiMasterWithChipSelect.hpp"
#include "services/tracer/SerialCommunicationOnSeggerRtt.hpp"
#include "services/tracer/StreamWriterOnSerialCommunication.hpp"
#include "services/tracer/Tracer.hpp"
#include "infra/util/AutoResetFunction.hpp"
#include "infra/util/ReallyAssert.hpp"
#include <array>

namespace application
{
    // STM32 implementation of platform::Platform for the NUCLEO-WB55RG.
    //
    // Timer budget. The part has five complementary channels in total — TIM1
    // CH1N/CH2N/CH3N, TIM16 CH1N, TIM17 CH1N — and only TIM1 and TIM2 decode
    // quadrature. Two motors each needing a full bridge take four of those five,
    // so TIM1 goes to the left motor and TIM16 plus TIM17 to the right, leaving
    // TIM2 as the only encoder timer. The second encoder is decoded in software.
    //
    // Console. TIM16_CH1N exists only on PB6 and TIM17_CH1N only on PB7, which are
    // also the only USART1 pins left once TIM1 has taken PA9. The CLI therefore
    // runs over SEGGER RTT across the existing SWD link rather than the ST-LINK
    // virtual COM port; no UART pin is free while both motors drive full bridges.
    class PlatformImpl final
        : public platform::Platform
    {
    public:
        PlatformImpl();

        hal::GpioPin& StatusLed() override;
        hal::SerialCommunication& Communication() override;
        services::Tracer& Tracer() override;
        platform::MotorBridge& LeftMotorBridge() override;
        platform::MotorBridge& RightMotorBridge() override;
        hal::SpiMaster& MotorDriverConfiguration() override;
        hal::GpioPin& MotorDriverFault() override;
        hal::GpioPin& MotorDriverStall() override;
        hal::AnalogToDigitalPin<infra::MilliVolt, uint32_t>& MotorDriverBackEmf() override;
        void Run() override;

    private:
        // A motor whose two half-bridges are two complementary channels of one
        // timer. They share an update event, so both legs change together.
        class SingleTimerMotorBridge final
            : public platform::MotorBridge
        {
        public:
            SingleTimerMotorBridge(uint8_t timerOneBasedIndex, infra::MemoryRange<const hal::PwmStmBase::ChannelConfig> channels, hal::GpioPinStm& breakPin, const hal::PwmStmBase::Config& config);

            void SetBaseFrequency(hal::Hertz baseFrequency) override;
            void Start(hal::Percent halfBridgeA, hal::Percent halfBridgeB) override;
            void Stop() override;

        private:
            hal::SynchronousPwmStm pwm;
        };

        // A motor whose half-bridges sit on two timers, because no single timer
        // has two complementary channels left. Started together, but without a
        // shared update event, so their edges are not phase-locked.
        class SplitTimerMotorBridge final
            : public platform::MotorBridge
        {
        public:
            SplitTimerMotorBridge(uint8_t timerA, infra::MemoryRange<const hal::PwmStmBase::ChannelConfig> channelsA, hal::GpioPinStm& breakPinA,
                uint8_t timerB, infra::MemoryRange<const hal::PwmStmBase::ChannelConfig> channelsB, hal::GpioPinStm& breakPinB,
                const hal::PwmStmBase::Config& config);

            void SetBaseFrequency(hal::Hertz baseFrequency) override;
            void Start(hal::Percent halfBridgeA, hal::Percent halfBridgeB) override;
            void Stop() override;

        private:
            hal::SynchronousPwmStm pwmA;
            hal::SynchronousPwmStm pwmB;
        };

        // The driver's back-EMF channel in millivolts. Written out rather than
        // composed from unit conversions because the scale depends on this board's
        // reference voltage, which no generic converter knows.
        class BackEmfPin final
            : public hal::AnalogToDigitalPin<infra::MilliVolt, uint32_t>
        {
        public:
            BackEmfPin(hal::GpioPinStm& pin, hal::AdcStm& adc)
                : impl(pin, adc)
            {}

            void Measure(SamplesRange samples, const infra::Function<void()>& onDone) override
            {
                really_assert(samples.size() <= maxSamples);

                this->samples = samples;
                this->onDone = onDone;

                impl.Measure(samples.size(), [this](infra::MemoryRange<uint16_t> raw)
                    {
                        for (std::size_t i = 0; i != this->samples.size(); ++i)
                            this->samples[i] = infra::Quantity<infra::MilliVolt, uint32_t>{ static_cast<uint32_t>(raw[i]) * referenceMilliVolt / fullScale };

                        this->onDone();
                    });
            }

        private:
            static constexpr std::size_t maxSamples = 16;
            static constexpr uint32_t referenceMilliVolt = 3300;
            static constexpr uint32_t fullScale = 4095;

            hal::AnalogToDigitalPinImplStm impl;
            SamplesRange samples;
            infra::AutoResetFunction<void()> onDone;
        };

        struct ClockInit
        {
            ClockInit()
            {
                HAL_Init();
                INVERTED_PENDULUM_BOT_ST_CLOCK_INIT();
            }
        };

        ClockInit clockInit;
        main_::StmEventInfrastructure eventInfrastructure;

        hal::GpioPinStm statusLed{ hal::Port::B, 0 };

        hal::GpioPinStm leftHighA{ hal::Port::A, 8 };    // TIM1_CH1
        hal::GpioPinStm leftLowA{ hal::Port::A, 7 };     // TIM1_CH1N
        hal::GpioPinStm leftHighB{ hal::Port::A, 9 };    // TIM1_CH2
        hal::GpioPinStm leftLowB{ hal::Port::B, 14 };    // TIM1_CH2N
        hal::GpioPinStm leftBreak{ hal::Port::B, 12 };   // TIM1_BKIN

        hal::GpioPinStm rightHighA{ hal::Port::B, 8 };   // TIM16_CH1
        hal::GpioPinStm rightLowA{ hal::Port::B, 6 };    // TIM16_CH1N
        hal::GpioPinStm rightBreakA{ hal::Port::B, 5 };  // TIM16_BKIN
        hal::GpioPinStm rightHighB{ hal::Port::B, 9 };   // TIM17_CH1
        hal::GpioPinStm rightLowB{ hal::Port::B, 7 };    // TIM17_CH1N
        hal::GpioPinStm rightBreakB{ hal::Port::B, 4 };  // TIM17_BKIN

        hal::GpioPinStm spiClock{ hal::Port::A, 5 };
        hal::GpioPinStm spiMiso{ hal::Port::A, 6 };
        hal::GpioPinStm spiMosi{ hal::Port::A, 12 };
        hal::GpioPinStm spiChipSelect{ hal::Port::A, 4 };
        hal::SpiMasterStm spi{ 1, spiClock, spiMiso, spiMosi };
        services::SpiMasterWithChipSelect spiWithChipSelect{ spi, spiChipSelect };

        hal::GpioPinStm motorDriverFault{ hal::Port::C, 4 };
        hal::GpioPinStm motorDriverStall{ hal::Port::C, 5 };
        hal::GpioPinStm backEmfPin{ hal::Port::A, 1 };
        hal::AdcStm adc{ 1 };
        BackEmfPin backEmf{ backEmfPin, adc };

        std::array<hal::PwmStmBase::ChannelConfig, 2> leftChannels;
        std::array<hal::PwmStmBase::ChannelConfig, 1> rightChannelsA;
        std::array<hal::PwmStmBase::ChannelConfig, 1> rightChannelsB;

        SingleTimerMotorBridge leftMotorBridge;
        SplitTimerMotorBridge rightMotorBridge;

        services::SerialCommunicationOnSeggerRtt communication;
        services::StreamWriterOnSerialCommunication::WithStorage<256> streamWriter{ communication };
        infra::TextOutputStream::WithErrorPolicy stream{ streamWriter };
        services::TracerToStream tracer{ stream };
    };
}

#endif
