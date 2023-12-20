
# Serial communications

There are different options for serial communications with SimpleFOC.

## Commander

Commander is part of the main SimpleFOC library, and enables a simple ASCII command interface to SimpleFOC. It also allows control over the standard motor "monitoring" functionality which can be used for basic telemetry.

Please see our docs on [Commander]() and [Monitoring]()

## Telemetry

A more complex abstraction which allows you to monitor most of SimpleFOCs parameters and state - anything that can be accessed via one of our [Registers](../). Telemetry data can be output to the serial port, in either ASCII or binary formats.

See [Telemetry](../telemetry/) and [Streams Comms](../streams/) for more documentation.

## PacketCommander

PacketCommander is a serial communications class for register-based control of SimpleFOC. It works with our [Registers](../) and can operate in ASCII or binary mode. It can be used in combination with the Telemetry class.

See [PacketCommander](../streams/)

## Python API

Our Python API can work via serial port on your PC with any of the above options. See [pysimplefoc](https://github.com/simplefoc/pysimplefoc)