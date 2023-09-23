
# SimpleFOC Telemetry

:warning: unfinished, untested

A flexible abstraction for telemetry (monitoring) of SimpleFOC systems.

The telemetry implementation is based on the SimpleFOC registers, and allows you to send telemetry for any (readable) register. Telemetry supports multiple motors.

The concept allows you to choose registers which are then sent by the telemetry automatically, on a regular schedule.

The method of sending depends on the type of telemetry you add to your program. There are telemetry drivers for:

- Serial ASCII telemetry
- Serial Binary telemetry
- and more drivers will be added in the future

Multiple motors can be added to the same telemetry, to monitor several motors at the same time. The registers reported by telemetry can be changed at run-time. Multiple instances of telemetry can be used to monitor different sets of values at different time intervals, or to send to multiple channels at the same time.

## Usage

Using telemetry is simple:

```c++

SerialASCIITelemetry telemetry = SerialASCIITelemetry();
...

void setup() {
    ...
    telemetry.addMotor(&motor);
    telemetry.setTelemetryRegisters(3, { REG_TARGET, REG_ANGLE, REG_VELOCITY });
    telemetry.init();
    ...
}

void loop() {
    motor.move();
    motor.loopFOC();
    telemetry.run();
}
```