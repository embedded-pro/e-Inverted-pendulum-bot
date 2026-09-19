Feature: Sensing, attitude estimation and wheel odometry
  The robot cannot balance better than it can measure. These scenarios pin down the
  quality of the pitch estimate and the wheel measurements the controller depends on,
  and the failure signalling that lets the safety supervisor act on bad data.

  @REQ-IMU-001
  Scenario: Inertial sampling keeps up with the control loop
    Given the robot is running
    When inertial samples are counted over 1 second
    Then at least 500 complete six-axis samples were acquired

  @REQ-IMU-005
  Scenario: A stalled sensor is reported as stale
    Given the robot is running
    When no new inertial sample arrives for 3 sampling periods
    Then the sensing output is marked invalid

  @REQ-IMU-006
  Scenario: A failed sensor transfer is not silently propagated
    Given the robot is running
    When an inertial sensor transfer fails
    Then the failure is reported
    And no measurement value is published for that sample

  @REQ-EST-001 @REQ-EST-002
  Scenario: The estimator publishes pitch and pitch rate
    Given the robot is held at a known inclination of 10 degrees
    When the estimator has converged
    Then the estimated pitch is within 1 degree of 10 degrees
    And the estimated pitch rate is within 2 degrees per second of zero

  @REQ-EST-003
  Scenario: The pitch estimate does not drift at rest
    Given the robot is stationary and upright
    When the robot runs continuously for 60 seconds
    Then the estimated pitch has drifted by no more than 1 degree

  @REQ-EST-005
  Scenario: The estimate carries an explicit validity indication
    Given the robot is running
    When the sensing output is marked invalid
    Then the attitude estimate is marked invalid
    And the invalid estimate is not presented as usable

  @REQ-EST-006
  Scenario: A fresh estimate is available to every control iteration
    Given the robot is running
    When a balance control iteration begins
    Then the attitude estimate it consumes was produced by the most recent inertial sample

  @REQ-EST-007
  Scenario: Linear acceleration does not tilt the estimate
    Given the robot is upright
    When the chassis is accelerated forwards for 500 milliseconds
    Then the estimated pitch deviates by no more than 2 degrees during the acceleration
    And the estimated pitch returns to within 1 degree of upright afterwards

  @REQ-ODOM-001 @REQ-ODOM-002
  Scenario: Each wheel is decoded independently in quadrature
    Given both wheel encoders are connected
    When the left wheel is turned through one full revolution
    Then the left wheel count changes by four times the encoder line count
    And the right wheel count is unchanged

  @REQ-ODOM-003
  Scenario: Forward motion is positive on both wheels
    Given the robot is pushed forwards
    When the wheel displacements are read
    Then the left wheel displacement is positive
    And the right wheel displacement is positive

  @REQ-ODOM-004
  Scenario: Wheel angular velocity is reported
    Given the left wheel is turning at a constant 5 revolutions per second
    When the wheel velocity is read at the control-loop rate
    Then the reported left wheel angular velocity is within 2 percent of 5 revolutions per second

  # Deferred: the second Then holds today - nothing consults the index, so it cannot disturb
  # the count - but the first is not yet implemented. The portable encoder abstraction has no
  # index accessor, and the hardware layer offers only a bare level read with no latch or
  # interrupt, so a once-per-revolution pulse cannot be caught by polling. Tracked as open
  # question 1 in documentation/design/wheel-odometry.md.
  @REQ-ODOM-005
  Scenario: The index pulse does not disturb the count
    Given the left wheel is turning continuously
    When the encoder index pulse occurs
    Then an index event is reported
    And the accumulated left wheel count is unaffected

  @REQ-ODOM-006
  Scenario: Counts survive hardware counter wrap-around
    Given the left wheel position is just below the hardware counter maximum
    When the wheel continues turning in the same direction past the counter maximum
    Then the accumulated position continues to increase monotonically
    And no counts are lost or duplicated

  @REQ-ODOM-007
  Scenario: Chassis motion is derived from the two wheels
    Given the left wheel turns forwards and the right wheel turns backwards at equal speed
    When the chassis motion is derived
    Then the reported forward velocity is approximately zero
    And the reported yaw rate is non-zero
