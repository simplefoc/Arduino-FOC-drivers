
# SimpleFOC communications support code

This folder contains classes to support you communicating between MCUs running SimpleFOC, and other systems.

## Telemetry

The [telemetry](telemetry/) subfolder contains classes which can log telemetry information for any of the SimpleFOC registers. Telemetry also supports multiple motors, and multiple telemetry objects can be used at the same time to log different values and at different frequencies.

Since registers are defined for most of SimpleFOC's settings and variables, this goes far beyond the standard monitoring capabilities of the motor class.

A telemetry implementation is provided that uses packet based IO, and is therefore compatible with PacketCommander, and can use either ASCII or Binary protocols (see below).

## PacketCommander

The [streams](streams/) subfolder contains PacketCommander, which allows reading or writing the SimpleFOC registers to control or query the motor. PacketCommander is based on packet IO, and can run in either ASCII or Binary mode.

## I2CCommander

Again based on the registers abstraction, [I2CCommander](i2c/) is a class which enables reading and writing of SimpleFOC registers via I2C bus. When running I2CCommander your MCU is in I2C target mode, and can then be controlled from another MCU or Raspberry Pi.

## Packet IO for Arduino Streams

[Packet based IO](streams/) is implemented for Arduino Stream objects - meaning they can be used with Serial, but potentially also with WiFi or BLE connections supporting the Streams API.

Implementations are available for either ASCII based protocol (TextIO) or binary protocol (BinaryIO).

## Registers abstraction

[SimpleFOCRegisters.h](./SimpleFOCRegisters.h) contains a list of registers known to SimpleFOC. These registers can be read and/or written and code is provided to serialize/deserialize them.

The SimpleFOC packet based IO (PacketCommander, Telemetry), I2CCommander and SettingsStorage as well as our Python API [PySimpleFOC](https://github.com/simplefoc/pysimplefoc) are all based on this register abstraction, and therefore share the same register names/ids.

If implementing your own communications protocol, we encourage you to base it on the Register abstraction if appropriate. This will provide you with ready to use code to access/write the register values, and will make your solution easier to use due to the shared common conventions.