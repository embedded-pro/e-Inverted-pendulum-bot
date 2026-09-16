Feature: Balancing and motion tracking
  The core behaviour: hold the body upright, and do so while following the operator's
  velocity and yaw commands. Every scenario here is written against observable
  behaviour, never against a particular control law, so that any strategy offered
  through the controller interface must satisfy them.

  Background:
    Given the robot is calibrated and in IDLE
    And the robot is standing on a level surface

  # REQ-SAFE-001
  Scenario: Arming requires a near-upright attitude
    Given the robot is held 20 degrees from upright
    When an arm command is issued
    Then the command is rejected
    And the mode remains IDLE

  # REQ-CTRL-001
  Scenario: The robot holds itself upright
    Given the robot is held within 5 degrees of upright
    When the robot is armed and released
    Then the robot remains upright for 30 seconds
    And the estimated pitch stays within 3 degrees of the pitch setpoint

  # REQ-CTRL-008
  Scenario: Arming resets the controller state
    Given the robot was previously armed and accumulated internal controller state
    And the robot was disarmed
    When the robot is armed again
    Then the active strategy starts from its reset state
    And the initial effort command does not reflect the previous session

  # REQ-CTRL-002
  Scenario: The robot recovers from a push
    Given the robot is armed and balancing
    When an impulsive disturbance displaces the body by 10 degrees from upright
    Then the estimated pitch returns to within 3 degrees of the setpoint within 1 second
    And the robot does not fall

  # REQ-CTRL-003
  Scenario: The robot tracks a forward velocity command
    Given the robot is armed and balancing
    When a forward velocity setpoint of 0.3 metres per second is commanded
    Then the measured chassis velocity settles at 0.3 metres per second
    And the estimated pitch stays within 3 degrees of the pitch setpoint

  # REQ-CTRL-004
  Scenario: The robot tracks a yaw rate command
    Given the robot is armed and balancing
    When a yaw rate setpoint of 45 degrees per second is commanded
    Then the measured yaw rate settles at 45 degrees per second
    And the robot remains upright

  # REQ-CTRL-009
  Scenario: Effort is saturated and does not wind up
    Given the robot is armed and balancing
    When the robot is held against a wall so that the wheels cannot turn for 3 seconds
    Then the commanded effort never exceeds the configured actuator range
    And on release the robot returns upright without an over-correcting lurch

  # REQ-CTRL-010
  Scenario: No effort is commanded when not armed
    Given the robot is in IDLE
    When the body is tilted by hand
    Then the commanded effort for both wheels is zero

  # REQ-DRIVE-002, REQ-DRIVE-003
  Scenario: Each bridge is driven directly and independently
    Given the robot is armed and balancing
    When the controller commands a different effort to each wheel
    Then each full bridge is driven directly by the firmware
    And the two bridges receive independent signed commands

  # REQ-DRIVE-008
  Scenario: Effort maps monotonically onto bridge duty
    Given the robot is armed
    When the commanded effort is increased through its range
    Then the resulting bridge duty cycle increases monotonically
    And the sign of the effort selects the bridge direction

  # REQ-DRIVE-007
  Scenario: Commutation is above the audible band
    Given the robot is armed
    When the bridge switching frequency is measured
    Then the switching frequency is above 20 kilohertz

  # REQ-RT-001, REQ-RT-002
  Scenario: The control loops run at their specified rates
    Given the robot is armed and balancing
    When loop iterations are counted over 1 second
    Then the balance loop executed 500 times
    And the velocity and yaw loop executed 50 times

  # REQ-RT-003
  Scenario: Loop timing is not jittery
    Given the robot is armed and balancing
    When the start time of each balance iteration is recorded over 1 second
    Then no iteration starts more than 10 percent of a period away from its nominal time

  # REQ-RT-004
  Scenario: Sensor to actuator latency is bounded
    Given the robot is armed and balancing
    When an inertial sample is acquired
    Then the resulting effort command is applied within 3 milliseconds

  # REQ-RT-005
  Scenario: The control path is bounded
    Given the balance control path
    When its execution is analysed
    Then it contains no recursion
    And it contains no unbounded iteration

  # REQ-RES-001, REQ-RES-002
  Scenario: The control path allocates nothing
    Given the robot is armed and balancing
    When 10000 balance iterations have executed
    Then no dynamic memory allocation has occurred since startup
    And every collection in use has a capacity fixed at compile time

  # REQ-RES-003
  Scenario: Stack usage fits the target
    Given the firmware built for the target microcontroller
    When the worst-case stack depth is analysed at build time
    Then the worst-case stack depth fits within the target memory budget
