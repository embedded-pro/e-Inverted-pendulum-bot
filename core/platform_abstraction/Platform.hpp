#ifndef CORE_PLATFORM_ABSTRACTION_PLATFORM_HPP
#define CORE_PLATFORM_ABSTRACTION_PLATFORM_HPP

#include "core/platform_abstraction/MotorBridge.hpp"
#include "hal/interfaces/AnalogToDigitalPin.hpp"
#include "hal/interfaces/Gpio.hpp"
#include "hal/interfaces/SerialCommunication.hpp"
#include "hal/interfaces/Spi.hpp"
#include "services/tracer/Tracer.hpp"

namespace platform
{
    // Abstract platform interface — the seam between portable application logic
    // (in core/) and a concrete board (in targets/platform_implementations/).
    //
    // Application code depends ONLY on this interface, so the same logic runs on
    // the host (for testing) and on each supported microcontroller. Each board
    // provides a `PlatformImpl : Platform` that wires real peripherals; the host
    // provides stubs. This is the single most important pattern in this project.
    class Platform
    {
    public:
        Platform() = default;
        Platform(const Platform& other) = delete;
        Platform& operator=(const Platform& other) = delete;

        // A digital output driving a status/heartbeat LED.
        virtual hal::GpioPin& StatusLed() = 0;

        // A bidirectional serial channel used by the command-line interface.
        virtual hal::SerialCommunication& Communication() = 0;

        // Text output (banner, command responses, terminal echo).
        virtual services::Tracer& Tracer() = 0;

        // The two motor bridges. Forward wheel motion is positive effort on both,
        // despite their mirrored mounting.
        virtual MotorBridge& LeftMotorBridge() = 0;
        virtual MotorBridge& RightMotorBridge() = 0;

        // Configuration channel to the motor driver, chip select already applied.
        virtual hal::SpiMaster& MotorDriverConfiguration() = 0;

        // The driver's fault output. Observable without polling the configuration
        // channel, so a fault is visible even when SPI is busy or broken.
        virtual hal::GpioPin& MotorDriverFault() = 0;

        // The driver's stall output, and its back-EMF sense channel.
        virtual hal::GpioPin& MotorDriverStall() = 0;
        virtual hal::AnalogToDigitalPin<infra::MilliVolt, uint32_t>& MotorDriverBackEmf() = 0;

        // Hand control to the platform's event loop. Never returns on hardware.
        virtual void Run() = 0;

    protected:
        ~Platform() = default;
    };
}

#endif
