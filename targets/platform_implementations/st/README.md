# platform_implementations/st

STM32 implementation of `platform::Platform`, shared by all supported ST board
presets: **NUCLEO-WB55RG**.

- **StatusLed** — LD2 (green) on PB0.
- **Communication** — `hal::UartStm` on USART1 (TX = PB6, RX = PB7), 115200 8N1.
  On the NUCLEO-WB55RG this UART is routed to the on-board ST-LINK virtual COM
  port, so no USB-UART adapter is needed.
- **Tracer** — `services::TracerToStream` over the UART.
- **Run** — runs `main_::StmEventInfrastructure`. A first member (`ClockInit`) calls
  `HAL_Init()` + the board's default clock configuration function before any
  peripheral is constructed (32 MHz HSE on the NUCLEO-WB55RG).

The default clock header/init function is selected per board in
`CMakeLists.txt` (via `INVERTED_PENDULUM_BOT_ST_CLOCK_HEADER` / `INVERTED_PENDULUM_BOT_ST_CLOCK_INIT`,
keyed off `TARGET_MCU`). To support another STM32 board, add a `TARGET_MCU`
case there pointing at the matching `hal_st` clock header/function, and add a
preset in `CMakePresets.json`.
