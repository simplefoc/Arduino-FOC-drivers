
# PacketCommander & Packet IO

Our packet based IO classes based on the SimpleFOC registers are a more flexible and powerful way to interact with your SimpleFOC code from the outside.

They are supported by our Python API [PySimpleFOC](https://github.com/simplefoc/pysimplefoc).

### PacketCommander

Use PacketCommander in combination with either TextIO (for ASCII text based protocol) or BinaryIO (for a binary protocol) to send control messages to SimpleFOC motors in the form of register reads and writes.

```c++
BLDCMotor motor = BLDCMotor(7); // 7 pole pairs
TextIO comms = TextIO(Serial);
PacketCommander packetCommander = PacketCommander();

void setup() {
    ...
    packetCommander.addMotor(&motor);
    packetCommander.echo = true;
    packetCommander.init(comms);
    ...
}

void run() {
    ...
    packetCommander.run();
}
```

You can then send messages to PacketCommander like this:

```
R2=0        # set motor 0 (note this is default if you only have 1 motor...)
R8=10.0     # set target to 10.0
R4=1        # enable motor

R0          # read register 0, the status register
```

The PacketCommander will respond with response messages like this:

```
r2=0        # motor address is 0
r8=10.0000  # target is 10
r4=1        # motor is enabled

r0=4        # motor status is FOCMotorStatus::motor_ready
```

## Streams IO

IO objects which work with Arduino Streams. Used for the Telemetry abstraction, or packet based control of SimpleFOC via PacketCommander.

This can be used in conjunction with Serial, probably the way most people will use it, but also with other Streams like TCP network connections.

There are Binary and Text versions of the Streams implementation. Binary will be a bit more compact, but isn't easily readable.

## Usage

Use of the IO classes is shown in conjunction with their use in PacketCommander or Telemetry. Please see the docs for those classes.

### TextIO

In TextIO, you can set the float precision:

```c++
TextIO comms = TextIO(Serial);

void setup(){
    comms.precision = 6; // print floats with 6 digits after the decimal place
}
```

### BinaryIO

BinaryIO has no options:

```c++
BinaryIO comms = BinaryIO(Serial);
```
