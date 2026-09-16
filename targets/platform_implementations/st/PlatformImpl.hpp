#ifndef TARGETS_PLATFORM_IMPLEMENTATIONS_ST_PLATFORM_IMPL_HPP
#define TARGETS_PLATFORM_IMPLEMENTATIONS_ST_PLATFORM_IMPL_HPP

#include "core/platform_abstraction/Platform.hpp"
#include "hal_st/instantiations/StmEventInfrastructure.hpp"
#include INVERTED_PENDULUM_BOT_ST_CLOCK_HEADER
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/stm32fxxx/UartStm.hpp"
#include "infra/stream/OutputStream.hpp"
#include "services/tracer/StreamWriterOnSerialCommunication.hpp"
#include "services/tracer/Tracer.hpp"

namespace application
{
    // STM32 implementation of platform::Platform, shared by all supported ST board
    // presets (see CMakePresets.json). The default clock configuration is selected
    // per board via INVERTED_PENDULUM_BOT_ST_CLOCK_HEADER/INVERTED_PENDULUM_BOT_ST_CLOCK_INIT (set in
    // targets/platform_implementations/st/CMakeLists.txt based on TARGET_MCU).
    //
    // - Status LED  : LD2 (green) on PB0
    // - CLI UART    : USART1, TX = PB6, RX = PB7 (115200 8N1). On the
    //                 NUCLEO-WB55RG this UART is routed to the on-board ST-LINK
    //                 virtual COM port, so no USB-UART adapter is needed.
    class PlatformImpl final
        : public platform::Platform
    {
    public:
        hal::GpioPin& StatusLed() override;
        hal::SerialCommunication& Communication() override;
        services::Tracer& Tracer() override;
        void Run() override;

    private:
        // Initialises the STM32 HAL and system clock before any peripheral member
        // is constructed. Declared first so its constructor runs first.
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
        hal::GpioPinStm uartTx{ hal::Port::B, 6 };
        hal::GpioPinStm uartRx{ hal::Port::B, 7 };
        hal::UartStm uart{ 1, uartTx, uartRx };
        services::StreamWriterOnSerialCommunication::WithStorage<256> streamWriter{ uart };
        infra::TextOutputStream::WithErrorPolicy stream{ streamWriter };
        services::TracerToStream tracer{ stream };
    };
}

#endif
