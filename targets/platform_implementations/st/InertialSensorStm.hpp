#pragma once

#include "core/platform_abstraction/InertialSensor.hpp"
#include "drivers/imu/mpu9250/Mpu9250BusAccessSpi.hpp"
#include "drivers/imu/mpu9250/Mpu9250Core.hpp"
#include "hal_st/stm32fxxx/GpioStm.hpp"
#include "hal_st/stm32fxxx/SpiMasterStm.hpp"
#include "services/peripheral/SpiMasterWithChipSelect.hpp"

namespace application
{
    class InertialSensorStm final
        : public platform::InertialSensor
    {
    public:
        struct AxisMap
        {
            uint8_t xFrom{ 0 };
            uint8_t yFrom{ 1 };
            uint8_t zFrom{ 2 };

            float xSign{ 1.0f };
            float ySign{ 1.0f };
            float zSign{ 1.0f };
        };

        InertialSensorStm();

        void Start(const infra::Function<void(const platform::InertialSample&)>& onSample) override;
        void Stop() override;

        bool Identified() const;

    private:
        static drivers::Mpu9250Core::Config DeviceConfig();
        static hal::SpiMasterStm::Config BusConfig();

        void StartSampling();
        platform::InertialAxes ToBodyFrame(float first, float second, float third) const;
        void OnAcceleration(drivers::Mpu9250Core::Accelerometer::Samples samples);
        void OnAngularVelocity(drivers::Mpu9250Core::Gyroscope::Samples samples);

        hal::GpioPinStm clock{ hal::Port::B, 13 };
        hal::GpioPinStm miso{ hal::Port::B, 14 };
        hal::GpioPinStm mosi{ hal::Port::B, 15 };
        hal::GpioPinStm chipSelect{ hal::Port::B, 12 };
        hal::GpioPinStm dataReady{ hal::Port::A, 10 };

        hal::SpiMasterStm spi{ 2, clock, miso, mosi, BusConfig() };
        services::SpiMasterWithChipSelect spiWithChipSelect{ spi, chipSelect };
        drivers::Mpu9250BusAccessSpi busAccess{ spiWithChipSelect };
        drivers::Mpu9250Core device{ busAccess, dataReady };

        AxisMap axisMap;
        infra::Function<void(const platform::InertialSample&)> onSample;
        platform::InertialSample pending;
        bool accelerationReceived{ false };
        bool identified{ false };
        bool initializing{ false };
    };
}
