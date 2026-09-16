---
title: "Motion Actuation Design"
type: design
status: draft
version: 0.1.0
component: "motion-actuation"
date: 2026-09-16
---

| Field     | Value                   |
|-----------|-------------------------|
| Title     | Motion Actuation Design |
| Type      | design                  |
| Status    | draft                   |
| Version   | 0.1.0                   |
| Component | motion-actuation        |
| Date      | 2026-09-16              |

> Turns an abstract effort command into current through two motors, and turns two encoder
> channel pairs back into measured motion. The one component that must always be able to
> stop, whatever else has failed.

---

## Responsibilities

**Is responsible for:**
- Configuring the motor driver at startup and verifying the configuration by read-back.
- Driving both full bridges directly, bypassing the driver's internal step sequencer.
- Mapping a signed effort command onto bridge duty cycle and direction.
- Providing coast and brake disable states, and guaranteeing coast is always reachable.
- Observing the driver's fault output and reporting it to the safety supervisor.
- Decoding both quadrature encoders into signed wheel position and velocity.
- Deriving chassis forward velocity and yaw rate from the two wheel velocities.

**Is NOT responsible for:**
- Deciding what effort to apply, or whether the drive may be energised.
- Interpreting a fault beyond latching and reporting it.
- Estimating body attitude.
- Closing a current or velocity loop around the motors; effort maps to duty open-loop, and
  the outer loops live in balance control.

---

## Component Details

### Part A — One driver, two brushed motors

The motor driver part is nominally a stepper driver, but a stepper driver *is* two
independent full H-bridges plus a step sequencer. Bypassing the sequencer and driving the
two bridges directly yields exactly what a two-wheeled balancer needs: two independently
commanded brushed DC motors, per-motor current regulation, and a single fault output, all
configured over one serial channel.

The consequence for this design is that bridge state is commanded by the firmware on every
control iteration rather than delegated to the part. Phasing and dead-time behaviour become
this component's concern.

### Part B — Configuration and verification

Driver configuration — current limit, decay mode, sequencer bypass — is written during INIT
and read back. A driver that does not read back what was written is treated as absent, not
as merely misconfigured: energising motors through a driver in an unknown state is the
failure mode this check exists to prevent.

### Part C — Effort to duty mapping

Effort arrives normalised and signed. Its magnitude selects duty cycle, its sign selects
bridge direction. The mapping is monotonic and documented, so that a change in commanded
effort always produces a change in the same direction at the wheel — a property the control
strategies rely on and none of them verify.

Switching frequency sits above the audible band and is matched to the motor's electrical
time constant: too low and the robot whines and the current ripples; too high and switching
losses dominate.

### Part D — Coast, brake, and why safety uses coast

Coasting opens both bridge legs, leaving the motor terminals floating; the robot's wheels
turn freely. Braking shorts the terminals, dissipating kinetic energy and resisting motion.

Safety-initiated disables always coast. A falling robot that brakes plants its wheels and
converts a topple into a harder impact, and braking still drives current through the
bridges at the moment a fault is suspected. Coast is also the state the hardware reaches
without firmware cooperation, which is what makes it reachable when the control loop is
already gone.

### Part E — Quadrature decoding

Each encoder's A and B channels are decoded in four-times quadrature, counting every edge
on both channels for maximum resolution. Counts accumulate into a wide software position
that is immune to the hardware counter wrapping. The index channel is reported as an event
for diagnostics and homing but is never allowed to reset the incremental count — a spurious
index pulse must not teleport the robot's odometry.

The two wheels are mirrored physically, so one wheel's raw count is negated before use; both
report positive displacement for forward robot motion.

### Part F — Chassis motion

Forward velocity is the mean of the two wheel velocities scaled by wheel radius; yaw rate is
their difference scaled by radius over track width. Both are derived here rather than in the
controller so that the wheel geometry is described in exactly one place.

---

## Interfaces

### Provided

| Interface | Purpose | Contract |
|-----------|---------|----------|
| Effort application | Apply a signed effort to each motor | Monotonic mapping to duty and direction; ignored unless the drive is permitted |
| Drive disable | Coast or brake both bridges | Coast must succeed without a healthy control loop; safety disables always coast |
| Driver health | Report driver-asserted faults | Latched on assertion, even if the condition clears immediately |
| Wheel measurement | Signed position and angular velocity per wheel | Lossless across counter wrap; forward motion positive on both wheels |
| Index events | Once-per-revolution marker per wheel | Reported without disturbing the accumulated count |
| Chassis motion | Forward velocity and yaw rate | Derived from both wheels and the documented geometry; updated at the control-loop rate |

### Required

| Interface | Purpose | Contract |
|-----------|---------|----------|
| Driver configuration channel | Write and read back driver configuration | Read-back mismatch is a fatal startup condition |
| Bridge control outputs | Duty and direction per bridge | Switching frequency above the audible band |
| Driver fault input | Observe the driver's fault assertion | Observable without polling the configuration channel |
| Encoder channel inputs | A, B and index per wheel | Decoded without losing edges at maximum wheel speed |
| Timebase | Velocity differencing interval | Monotonic; the measured interval is used for differencing |

---

## Data Model

| Entity | Field | Type / Unit | Range | Notes |
|--------|-------|-------------|-------|-------|
| Command | effortLeft, effortRight | normalised effort | -1.0 to 1.0 | Sign selects direction |
| Command | disableState | enumeration | Coast, Brake | Safety paths use Coast exclusively |
| Wheel measurement | position | encoder counts | full signed range | Accumulated across hardware wrap |
| Wheel measurement | angularVelocity | radians per second | -105 to 105 | Differenced over the measured interval |
| Chassis motion | forwardVelocity | metres per second | -1.5 to 1.5 | Mean of both wheels times wheel radius |
| Chassis motion | yawRate | radians per second | -3.1 to 3.1 | Wheel difference times radius over track width |
| Geometry | wheelRadius | metres | fitted value | Documented in one place only |
| Geometry | trackWidth | metres | fitted value | Distance between wheel contact patches |
| Geometry | countsPerRevolution | counts | fitted value | Four times the encoder line count |
| Configuration | currentLimit | amperes | at or below the motor continuous rating | Verified by read-back |
| Configuration | switchingFrequency | kilohertz | above 20 | Above the audible band |

---

## State Machine

```mermaid
stateDiagram-v2
    [*] --> Unconfigured
    Unconfigured --> Configured : Configuration written and read back
    Unconfigured --> Failed : Read-back mismatch or no response
    Configured --> Coasting : Bridges enabled, zero effort
    Coasting --> Driving : Drive permitted, non-zero effort
    Driving --> Coasting : Drive permission withdrawn
    Driving --> Braking : Brake requested
    Braking --> Coasting : Brake released
    Driving --> Faulted : Driver asserted fault
    Coasting --> Faulted : Driver asserted fault
    Faulted --> Coasting : Fault cleared by the supervisor
    Failed --> [*]
```

---

## Sequence Diagrams

Startup configuration.

```mermaid
sequenceDiagram
    participant Sup as Safety supervisor
    participant Act as Motion actuation
    participant Drv as Motor driver

    Sup->>Act: Initialise
    Act->>Drv: Write configuration (sequencer bypassed, current limit)
    Act->>Drv: Read configuration back
    Drv-->>Act: Configuration
    alt Matches
        Act-->>Sup: Ready, bridges coasting
    else Mismatch
        Act-->>Sup: Driver identification failed
    end
```

A driver fault during flight.

```mermaid
sequenceDiagram
    participant Drv as Motor driver
    participant Act as Motion actuation
    participant Sup as Safety supervisor

    Drv->>Act: Fault asserted
    Act->>Act: Latch cause
    Act->>Drv: Disable both bridges (coast)
    Act-->>Sup: Driver fault
    Sup->>Sup: Mode = FAULT
    Note over Act: Latch persists even if the driver deasserts immediately
```

---

## Block Diagram

```mermaid
graph LR
    EFFORT[Effort command] --> MAP[Effort to duty and direction]
    MAP --> BRIDGE[Full bridge per motor]
    BRIDGE --> MOTOR[Brushed DC motors]
    MOTOR --> ENC[Quadrature encoders A, B, Z]
    ENC --> DEC[Four-times decode and accumulate]
    DEC --> WHEEL[Per-wheel position and velocity]
    WHEEL --> CHASSIS[Forward velocity and yaw rate]
    CFG[Configuration channel] --> BRIDGE
    BRIDGE -->|fault| LATCH[Fault latch]
    LATCH --> SUP[To safety supervisor]
    SUP -->|coast| BRIDGE
```

---

## Constraints & Limitations

| Constraint | Value / Description |
|------------|---------------------|
| Coast reachability | Coasting must not depend on the control loop, the estimator or the link |
| Configuration trust | An unverifiable driver configuration prevents startup rather than degrading operation |
| Switching frequency | Above 20 kHz and compatible with the motor electrical time constant |
| Current limit | At or below the continuous rating of the fitted motors |
| Open-loop torque | Effort maps to duty, not to current. Torque per unit effort varies with battery voltage and motor speed; the balance loops absorb this |
| No battery compensation | A discharging battery reduces the effort actually delivered; not compensated in this revision |
| Index is advisory | The index channel never corrects the incremental count, so absolute wheel angle is not established by this component |
| Slip is invisible | Odometry measures wheel rotation, not ground motion. A slipping wheel reports travel that did not happen |

---

## Open Questions

| # | Question | Options | Status |
|---|----------|---------|--------|
| 1 | Is quadrature decoded by hardware timers or in software on edge interrupts? | Hardware timer per wheel; software edge counting | open |
| 2 | Should effort compensate for measured battery voltage so torque per unit effort stays constant as the battery drains? | Leave to the balance loops; add feed-forward compensation | open |
| 3 | Fast or slow current decay mode for the bridges? | Depends on measured current ripple against motor inductance | open |
| 4 | Should wheel velocity be differenced per control period or filtered over several? | Per period, accepting quantisation noise; short moving filter, accepting lag | open |
| 5 | Should the driver's current regulation be relied upon, or a separate measurement taken? | Rely on the driver; add sensing for telemetry and stall detection | open |
