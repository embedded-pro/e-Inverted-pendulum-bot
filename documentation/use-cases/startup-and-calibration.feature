Feature: Startup and calibration
  From power-on the robot proves its hardware is present and healthy, learns the
  resting bias of its gyroscope, and settles in IDLE with the drive de-energised.
  No motor may turn until an operator explicitly arms the robot.

  Background:
    Given a robot fitted with an inertial sensor, a motor driver and two wheel encoders

  @REQ-SYS-002
  Scenario: Power-on leaves the drive de-energised
    Given the robot is powered off
    When the robot is powered on
    Then the mode is INIT
    And both motor bridges are disabled
    And no motor has been energised

  @REQ-SYS-003 @REQ-DRIVE-001
  Scenario: Self-test confirms the peripherals respond
    Given the robot is powered off
    When the robot is powered on
    Then the inertial sensor is identified successfully
    And the motor driver configuration is written and read back unchanged
    And the mode becomes CALIBRATING

  @REQ-SYS-003
  Scenario: A missing inertial sensor is fatal
    Given the inertial sensor does not respond to an identification read
    When the robot is powered on
    Then the mode becomes FAULT
    And the reported fault cause is sensor identification failure

  @REQ-SYS-005
  Scenario: Startup completes promptly
    Given the robot is powered off
    When the robot is powered on
    Then the mode leaves INIT within 2 seconds

  @REQ-IMU-002 @REQ-IMU-007
  Scenario: The inertial sensor is configured for the expected ranges
    When the robot completes INIT
    Then the angular-rate range is at least 500 degrees per second
    And the acceleration range is at least 4 g
    And measurements are reported in the documented body-frame axis convention

  @REQ-IMU-003 @REQ-EST-004
  Scenario: Bias calibration succeeds when the robot is held still
    Given the robot is held still and upright
    When the robot enters CALIBRATING
    Then a gyroscope bias is estimated for each axis
    And the estimated pitch converges to within 1 degree of the true inclination within 1 second
    And the mode becomes IDLE

  @REQ-IMU-004
  Scenario: Bias calibration is abandoned when the robot is moving
    Given the robot is being moved during the calibration window
    When the robot enters CALIBRATING
    Then calibration reports failure
    And the mode does not become IDLE

  @REQ-SYS-001 @REQ-SYS-004
  Scenario: Only defined transitions are accepted
    Given the mode is IDLE
    When a clear-fault command is issued
    Then the command is rejected
    And the mode remains IDLE

  @REQ-SYS-006
  Scenario: The robot reports its identity
    Given the robot is in IDLE
    When a client requests the firmware identification
    Then a firmware version identifier is reported
    And a build identifier is reported

  @REQ-TUNE-007
  Scenario: Persisted tuning is restored on power-on
    Given a control strategy and parameter set were stored before the last power-down
    When the robot is powered on
    Then the stored strategy is the active control strategy
    And the stored parameter values are restored

  @REQ-SAFE-009
  Scenario: An unexpected reset while armed comes back safe
    Given the robot is ARMED
    When the microcontroller resets unexpectedly
    Then both motor bridges are disabled
    And the mode is INIT

  @REQ-RES-004
  Scenario: The application logic runs without a microcontroller
    Given the host build of the application
    When the balance application is started against the host platform
    Then it initialises without requiring any microcontroller peripheral
