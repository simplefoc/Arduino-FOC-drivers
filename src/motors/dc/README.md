# SimpleFOC DCMotor


:warning: code in development! Please help us test it!


**Control DC motors using SimpleFOC's infrastructure.**

SimpleFOC is really intended for field oriented control of PMSM motors, it's in the name ;-). But for different reasons, sometimes DC motors are preferred, and while we're not focused on this use-case, we do have a fair amount of code that can be leveraged to help with DC-Motor control applications.

So this represents a less-supported effort to provide some useful building blocks for DC-Motors.

## DCMotor

This directory contains an initial implementation of a DCMotor class for closed loop control of DC motors using SimpleFOC's infrastructure.

It supports closed loop motor control based on a position sensor on the motor shaft, with the following modes:

- Torque-voltage control
- Velocity control
- Position control
- Torque-current control (:warning: not yet implemented)

It is intended to be used with one of the [DCDriver classes](../../drivers/dc/).

You can use any of the SimpleFOC sensor classes from [this repository](../../encoders/) or the [main repository](https://github.com/simplefoc/Arduino-FOC/tree/master/src/sensors).

### Open loop control

Open loop control is *not supported* by this class. But you can open loop control a DC motor just by directly using the DCDriver class and its `driver.setPwm(volts)` method.

### Usage

```c++
DCDriver driver = DCDriver2PWM(1,2,3);
Sensor sensor = MagneticSensorAS5048A(10);

void setup() {
    sensor.init();
    driver.init();
    motor.linkSensor(&sensor);
    motor.linkDriver(&driver);
    motor.controller = MotionControlType::torque;
    motor.torque_controller = TorqueControlType::voltage;
    motor.init();
}

void loop() {
    motor.move(5); // 5 Volts
}
```

More detailed examples can be found [here](../../../examples/motors/dc/).
