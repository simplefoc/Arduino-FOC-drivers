# SimpleFOC Driver and Support Library

![Library Compile](https://github.com/simplefoc/Arduino-FOC-drivers/workflows/Library%20Compile/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![Release](https://www.ardu-badge.com/badge/SimpleFOCDrivers.svg?)

This library contains an assortment of drivers and supporting code for SimpleFOC.

The intent is to keep the core of SimpleFOC clean, and thus easy to maintain, understand and port to different platforms. In addition to this core, there are various drivers and supporting code which has grown around SimpleFOC, and which we would like to make available to the community.

## New Release

v1.0.6 - Released July 2023, for Simple FOC 2.3.1 or later

What's changed since 1.0.5?
- Added AS5600 Sensor Driver
- Bugfixes [included](https://github.com/simplefoc/Arduino-FOC-drivers/issues?q=is%3Aissue+milestone%3A1.0.6+)

## What is included

What is here? See the sections below. Each driver or function should come with its own more detailed README.

### Motor/Gate driver ICs

 - [TMC6200 driver](src/drivers/tmc6200/) - SPI driver for Trinamics TMC6200 motor driver IC.
 - [DRV8316 driver](src/drivers/drv8316/) - SPI driver for TI's DRV8316 motor driver IC.

### Encoders

 - [AS5048A SPI driver](src/encoders/as5048a/) - SPI driver for the AMS AS5048A absolute position magnetic rotary encoder IC.
 - [AS5047 SPI driver](src/encoders/as5047/) - SPI driver for the AMS AS5047P and AS5047D absolute position magnetic rotary encoder ICs.
 - [AS5047U SPI driver](src/encoders/as5047u/) - SPI driver for the AMS AS5047U absolute position magnetic rotary encoder ICs.
 - [AS5600 I2C driver](src/encoders/as5600/) - I2C driver for the AMS AS5600 and AS5600L absolute position magnetic rotary encoder ICs.
 - [MA730 SPI driver](src/encoders/ma730/) - SPI driver for the MPS MagAlpha MA730 absolute position magnetic rotary encoder IC.
 - [MA730 SSI driver](src/encoders/ma730/) - SSI driver for the MPS MagAlpha MA730 absolute position magnetic rotary encoder IC.
 - [AS5145 SSI driver](src/encoders/as5145/) - SSI driver for the AMS AS5145 and AS5045 absolute position magnetic rotary encoder ICs.
 - [TLE5012B SPI driver](src/encoders/tle5012b/) - SPI (half duplex) driver for TLE5012B absolute position magnetic rotary encoder IC.
 - [STM32 Hardware Encoder](src/encoders/stm32hwencoder/) - Hardware timer based encoder driver for ABI type quadrature encoders.
 - [SC60228 SPI driver](src/encoders/sc60228/) - SPI driver for SemiMent SC60288 magnetic encoder IC.
 - [MA330 SPI driver](src/encoders/ma330/) - SPI driver for the MPS MagAlpha MA330 absolute position magnetic rotary encoder IC.
 - [MT6816 SPI driver](src/encoders/mt6816/) - SPI driver for the MagnTek MT6816 absolute position magnetic rotary encoder IC.
 - [MT6701 SSI driver](src/encoders/mt6701/) - SSI driver for the MagnTek MT6701 absolute position magnetic rotary encoder IC.
 - [MT6835 SPI driver](src/encoders/mt6835/) - SPI driver for the MagnTek MT6835 21 bit magnetic rotary encoder IC.
 - [STM32 PWM sensor driver](src/encoders/stm32pwmsensor/) - STM32 native timer-based driver for PWM angle sensors.
 - [SmoothingSensor](src/encoders/smoothing/) - A SimpleFOC Sensor wrapper implementation which adds angle extrapolation.

### Communications

 - [I2CCommander I2C driver](src/comms/i2c/) - I2C communications protocol and drivers for both controller and target devices, based on register abstraction
 - [STM32 SpeedDir Input](src/comms/stm32speeddir/) - Control target velocity with PWM speed and direction inputs
 - [SerialBinaryCommander](src/comms/serial/) - Serial communications with binary protocol, based on register abstraction
 - [Telemetry](src/comms/telemetry/) - Telemetry abstraction, based on registers
 - [SerialASCIITelemetry](src/comms/serial/) - Serial communications with ascii protocol, based on register abstraction

### Settings

Load and store SimpleFOC motor settings, based on register abstraction.

 - [SAMD NVM storage driver](src/settings/samd/) - Store settings to the NVM flash memory in your SAMD MCU
 - [CAT24 I2C EEPROM storage driver](src/settings/i2c/) - Store settings to CAT24 I2C EEPROMs

### Utilities

 - [STM32 PWM Input driver](src/utilities/stm32pwm/) - PWM Input driver for STM32 MCUs. Accurately measure PWM inputs with zero MCU overhead.


## How to use

#### Arduino Library Manager 
The simplest way to get hold of the library is directly by using Arduino IDE and its integrated Library Manager. 
- Open Arduino IDE and start Arduino Library Manager by clicking: `Tools > Manage Libraries...`.
- Search for `Simple FOC drivers` library and install the latest version.
- Reopen Arduino IDE and you should have the library examples in `File > Examples > Simple FOC drivers`.

#### Using Github website 
- Go to the [github repository](https://github.com/simplefoc/Arduino-FOC-drivers)
- Click first on `Clone or Download > Download ZIP`. 
- Unzip it and place it in `Arduino Libraries` folder. Windows: `Documents > Arduino > libraries`.  
- Reopen Arduino IDE and you should have the library examples in `File > Examples > Simple FOC drivers`.

#### Using parts

You can copy parts of the library, for example to minimize your code size, or make it easier to add adaptations of your own.
If you do so, please be sure to adhere to and include the [LICENSE](https://github.com/simplefoc/Arduino-FOC-drivers/LICENSE).


## Further Documentation

Find out more information about the Arduino SimpleFOC project on the [docs website](https://docs.simplefoc.com/) 

## Release History

What's changed since 1.0.4?
- Added smoothing sensor by [@dekutree64](https://github.com/dekutree64)
- Added TMD6200 SPI driver by [@YaseenTwati](https://github.com/YaseenTwati)
- Added HybridStepperMotor by [@VIPQualityPost](https://github.com/VIPQualityPost)
- New Settings abstraction to load and save SimpleFOC settings and calibration
- New Settings driver: SAMDNVMSettingsStorage
- SimpleFOCRegisters abstraction, mapping SimpleFOC parameters to virtual "Registers"
- Updated I2CCommander to use the new registers abstraction
- Bugfixes [included](https://github.com/simplefoc/Arduino-FOC-drivers/issues?q=is%3Aissue+milestone%3A1.0.5+)

What's changed since 1.0.3?
- New Comms/Input: STM32SpeedDirCommander
- New Utility: STM32PWMInput
- Fixed MT6835 driver bugs
- Improved AS5047 driver, fixed bugs
- Improved AS5047U driver, fixed bugs

What's changed since 1.0.2?
- New Sensor: MT6835
- Fixed bugs

What's changed since 1.0.1?
- Calibrated sensor by @MarethyuPrefect
- New Sensors: MT6701, MA330, MT6816
- Fixed bugs
