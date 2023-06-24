
# Serial communications classes

Serial communications classes for register-based control and telemetry from SimpleFOC.

## SerialASCIITelemetry

:warning: unfinished, untested

Telemetry class that sends telemetry as ASCII on a serial port. Similar to the classic "monitoring" functionality of SimpleFOC, but allows you to configure telemetry based on most of the defined registers.

Usage:

```c++

SerialASCIITelemetry telemetry = SerialASCIITelemetry(); // number of float digits to display

void setup() {
    ...
    telemetry.addMotor(&motor);
    telemetry.setTelemetryRegisters(2, [REG_VELOCITY, REG_VOLTAGE_Q]);
    telemetry.init();
    ...
}

void loop() {
    motor.move();
    motor.loopFOC();
    telemetry.run();
}
```

Some options are supported:

```c++
    telemetry.floatPrecision = 4;       // send floats with 4 decimal places
    telemetry.min_elapsed_time = 0;     // microseconds between sending telemetry
    telemetry.downsample = 100;         // send every this many loop iterations
```



## SerialBinaryCommander

:warning: unfinished, untested!

Control SimpleFOC via a binary protocol over the serial port. The standard SimpleFOC registers are used.

TODO document the protocol