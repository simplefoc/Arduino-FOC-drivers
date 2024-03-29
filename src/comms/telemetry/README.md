
# SimpleFOC Telemetry

:warning: new code, not yet fully tested

A flexible abstraction for telemetry (monitoring) of SimpleFOC systems.

The telemetry implementation is based on the SimpleFOC registers, and allows you to send telemetry for any (readable) register. Telemetry supports multiple motors.

The concept allows you to choose registers which are then sent by the telemetry automatically, on a regular schedule.

Multiple motors can be added to the same telemetry, to monitor several motors at the same time. The registers reported by telemetry can be changed at run-time. 

In addition, multiple instances of telemetry can be used to monitor different sets of values at different time intervals, or to send to multiple channels at the same time.

Each telemetry instance has an id (starting at 0) and can be controlled via register commands, for example using PacketCommander.

Each telemetry has the following configuration options:

- Motors added (at least one motor must be added)
- Register list to output, each register is associated with a motor
- Downsample rate, downsample=0 switches off the telemetry instance
- min_elapsed_time, telemetry is not sent more often than this

Further details and usage examples are provided for the individual implementations, below.


# SimpleTelemetry

Suitable when you want to plot your telemetry values using the arduino serial plotter, or similar tools.

The chosen register values will be printed as ASCII text, all on the same line, seperated by tabs and terminated by a newline character.

Note that this will most likely not work with registers that return more than one value, as these will be comma-separated.

## Usage

Using SimpleTelemetry:

```c++
TextIO io = TextIO(Serial);
SimpleTelemetry telemetry = SimpleTelemetry();
...

void setup() {
    ...
    telemetry.addMotor(&motor);
    telemetry.setTelemetryRegisters(3, { REG_TARGET, REG_ANGLE, REG_VELOCITY });
    telemetry.init(io);
    telemetry.downsample = 500;
    ...
}

void loop() {
    motor.move();
    motor.loopFOC();
    telemetry.run();
}
```


# Telemetry

The telemetry uses the packet based IO, so you can initialize it in either Text or Binary mode.

The telemetry data will be sent in packets at the configured rate. In ASCII mode, each packet will look something like this:

`T0=17.0982,1.1026`

This would be for telemetry id 0, with two values (floating point). If you don't know in advance, then to know the meaning of the values you need the telemetry header information.

The telemetry will send a header packet describing the registers contained in the subsequent telemetry packets. The header packet is sent again each time you change the registers. You can also force sending of the header by reading the REG_TELEMETRY_REG register (e.g. via PacketCommander). This will send the registers as a response packet, and additionally cause the telemetry to re-send the header packet.

In ASCII mode, the Header Packet will look something like this:

`H0=0:9,0:17`

Meaning header for telemetry id 0, with registers 9 (angle) and 17 (velocity), both for motor 0.

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

# TeleplotTelemetry

Formats the telemetry for plotting with [Teleplot](https://github.com/nesnes/teleplot/tree/main/vscode), from inside Visual Studio Code.

Using TeleplotTelemetry:

```c++
TextIO io = TextIO(Serial);
TeleplotTelemetry telemetry = TeleplotTelemetry();
...

void setup() {
    ...
    telemetry.addMotor(&motor);
    telemetry.setTelemetryRegisters(3, { REG_TARGET, REG_ANGLE, REG_VELOCITY });
    telemetry.init(io);
    telemetry.downsample = 500;
    ...
}

void loop() {
    motor.move();
    motor.loopFOC();
    telemetry.run();
}
```
