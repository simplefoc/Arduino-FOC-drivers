
# SimpleFOC Telemetry

:warning: new code, not yet fully tested

A flexible abstraction for telemetry (monitoring) of SimpleFOC systems.

The telemetry implementation is based on the SimpleFOC registers, and allows you to send telemetry for any (readable) register. Telemetry supports multiple motors.

The concept allows you to choose registers which are then sent by the telemetry automatically, on a regular schedule.

The telemetry uses the packet based IO, so you can initialize it in either Text or Binary mode.

Multiple motors can be added to the same telemetry, to monitor several motors at the same time. The registers reported by telemetry can be changed at run-time. Multiple instances of telemetry can be used to monitor different sets of values at different time intervals, or to send to multiple channels at the same time.

## Usage

Using telemetry is simple:

```c++
TextIO io = TextIO(Serial);
Telemetry telemetry = Telemetry();
...

void setup() {
    ...
    telemetry.addMotor(&motor);
    telemetry.setTelemetryRegisters(3, { REG_TARGET, REG_ANGLE, REG_VELOCITY });
    telemetry.init(io);
    ...
}

void loop() {
    motor.move();
    motor.loopFOC();
    telemetry.run();
}
```