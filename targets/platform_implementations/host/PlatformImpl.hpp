#ifndef TARGETS_PLATFORM_IMPLEMENTATIONS_HOST_PLATFORM_IMPL_HPP
#define TARGETS_PLATFORM_IMPLEMENTATIONS_HOST_PLATFORM_IMPL_HPP

#include "core/platform_abstraction/Platform.hpp"
#include "hal/generic/TimerServiceGeneric.hpp"
#include "hal/interfaces/Spi.hpp"
#include "infra/event/EventDispatcherWithWeakPtr.hpp"
#include "infra/stream/StringOutputStream.hpp"
#include "services/peripheral/SerialCommunicationLoopback.hpp"
#include "services/tracer/Tracer.hpp"
#include <algorithm>

namespace application
{
    // Host implementation of platform::Platform. It uses stubs and host facilities
    // (a wall-clock timer service, a loopback serial channel) so the same
    // application can be built and exercised off-target.
    class PlatformImpl final
        : public platform::Platform
    {
    public:
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
        // A do-nothing GPIO so DebugLed has an output to drive on the host.
        class GpioStub final
            : public hal::GpioPin
        {
        public:
            bool Get() const override
            {
                return false;
            }
            void Set(bool) override
            {}
            bool GetOutputLatch() const override
            {
                return false;
            }
            void SetAsInput() override
            {}
            bool IsInput() const override
            {
                return false;
            }
            void Config(hal::PinConfigType) override
            {}
            void Config(hal::PinConfigType, bool) override
            {}
            void ResetConfig() override
            {}
            void EnableInterrupt(const infra::Function<void()>&, hal::InterruptTrigger, hal::InterruptType) override
            {}
            void DisableInterrupt() override
            {}
        };

        // Accepts every command and drives nothing, so the whole control stack is
        // constructible on the host. A simulated plant replaces these later.
        class MotorBridgeStub final
            : public platform::MotorBridge
        {
        public:
            void SetBaseFrequency(hal::Hertz) override
            {}

            void Start(hal::Percent, hal::Percent) override
            {}

            void Stop() override
            {}
        };

        class SpiMasterStub final
            : public hal::SpiMaster
        {
        public:
            void SendAndReceive(infra::ConstByteRange, infra::ByteRange receiveData, hal::SpiAction, const infra::Function<void()>& onDone) override
            {
                std::ranges::fill(receiveData, 0);
                onDone();
            }

            void SetChipSelectConfigurator(hal::ChipSelectConfigurator&) override
            {}

            void SetCommunicationConfigurator(hal::CommunicationConfigurator&) override
            {}

            void ResetCommunicationConfigurator() override
            {}
        };

        class AnalogToDigitalPinStub final
            : public hal::AnalogToDigitalPin<infra::MilliVolt, uint32_t>
        {
        public:
            void Measure(SamplesRange samples, const infra::Function<void()>& onDone) override
            {
                std::ranges::fill(samples, infra::Quantity<infra::MilliVolt, uint32_t>{ 0 });
                onDone();
            }
        };

        infra::EventDispatcherWithWeakPtr::WithSize<50> eventDispatcher;
        hal::TimerServiceGeneric timerService;
        GpioStub led;
        services::SerialCommunicationLoopback loopback;
        infra::StringOutputStream::WithStorage<1024> stream;
        services::TracerToStream tracer{ stream };
        MotorBridgeStub leftMotorBridge;
        MotorBridgeStub rightMotorBridge;
        SpiMasterStub motorDriverSpi;
        GpioStub motorDriverFault;
        GpioStub motorDriverStall;
        AnalogToDigitalPinStub motorDriverBackEmf;
    };
}

#endif
