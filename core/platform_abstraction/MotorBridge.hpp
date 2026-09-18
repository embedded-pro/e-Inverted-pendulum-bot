#ifndef CORE_PLATFORM_ABSTRACTION_MOTOR_BRIDGE_HPP
#define CORE_PLATFORM_ABSTRACTION_MOTOR_BRIDGE_HPP

#include "hal/synchronous_interfaces/SynchronousPwm.hpp"

namespace platform
{
    // One motor's full bridge, presented as its two half-bridges.
    //
    // Each half-bridge is a duty cycle; the board decides how those reach the
    // gate driver, so a board using one timer with two complementary channels and
    // a board using two timers with one each look the same from here.
    //
    // The mapping the application relies on:
    //   forward   duty on A, zero on B
    //   reverse   zero on A, duty on B
    //   brake     zero on both, outputs still driven, so the motor is shorted
    //   coast     Stop(), outputs released, so the motor terminals float
    class MotorBridge
    {
    public:
        MotorBridge() = default;
        MotorBridge(const MotorBridge& other) = delete;
        MotorBridge& operator=(const MotorBridge& other) = delete;

        virtual void SetBaseFrequency(hal::Hertz baseFrequency) = 0;
        virtual void Start(hal::Percent halfBridgeA, hal::Percent halfBridgeB) = 0;

        // Releases both outputs. Must not depend on a healthy control loop.
        virtual void Stop() = 0;

    protected:
        ~MotorBridge() = default;
    };
}

#endif
