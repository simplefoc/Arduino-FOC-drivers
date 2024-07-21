# SimpleFOC Driver and Support Library

![Library Compile](https://github.com/simplefoc/Arduino-FOC-drivers/workflows/Library%20Compile/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![Release](https://www.ardu-badge.com/badge/SimpleFOCDrivers.svg?)

This library contains an assortment of drivers and supporting code for SimpleFOC.

The intent is to keep the core of SimpleFOC clean, and thus easy to maintain, understand and port to different platforms. In addition to this core, there are various drivers and supporting code which has grown around SimpleFOC, and which we would like to make available to the community.

## New Release

v1.0.8 - Released July 2024, for Simple FOC 2.3.4 or later


What's changed since 1.0.7?
- MA735 driver thanks to [@techyrobot](https://github.com/techy-robot)
- ESP32HWEncoder driver thanks to [@mcells](https://github.com/mcells)
- Bugfixes [included](https://github.com/simplefoc/Arduino-FOC-drivers/issues?q=milestone%3A1.0.8+)


## What is included

What is here? See the sections below. Each driver or function should come with its own more detailed README.

### Motor/Gate driver ICs

Software to control gate driver ICs or integrated driver ICs which have advanced configuration and status interfaces, e.g. via I2C or SPI.

 - [TMC6200 driver](src/drivers/tmc6200/) - SPI driver for Trinamics TMC6200 motor driver IC.
 - [DRV8316 driver](src/drivers/drv8316/) - SPI driver for TI's DRV8316 motor driver IC.
 - [STSPIN32G4 driver](src/drivers/stspin32g4/) - I2C and BLDCDriver for the STSPIN32G4 integrated gate driver MCU.

### Encoders

Drivers for various position sensor ICs. In many cases these hardware-specific drivers often support more functionality than the generic SimpleFOC sensor drivers, including reading status registers, setting configurations and more.

 - [AS5048A SPI driver](src/encoders/as5048a/) - SPI driver for the AMS AS5048A absolute position magnetic rotary encoder IC.
 - [AS5047 SPI driver](src/encoders/as5047/) - SPI driver for the AMS AS5047P and AS5047D absolute position magnetic rotary encoder ICs.
 - [AS5047U SPI driver](src/encoders/as5047u/) - SPI driver for the AMS AS5047U absolute position magnetic rotary encoder ICs.
 - [AS5600 I2C driver](src/encoders/as5600/) - I2C driver for the AMS AS5600 and AS5600L absolute position magnetic rotary encoder ICs.
 - [MA730 SPI driver](src/encoders/ma730/) - SPI driver for the MPS MagAlpha MA730 absolute position magnetic rotary encoder IC.
 - [MA730 SSI driver](src/encoders/ma730/) - SSI driver for the MPS MagAlpha MA730 absolute position magnetic rotary encoder IC.
 - [MA735 SPI driver](src/encoders/ma735/) - SPI driver for the MPS MagAlpha MA735 absolute position magnetic rotary encoder IC.
 - [MA735 SSI driver](src/encoders/ma735/) - SSI driver for the MPS MagAlpha MA735 absolute position magnetic rotary encoder IC. 
 - [AS5145 SSI driver](src/encoders/as5145/) - SSI driver for the AMS AS5145 and AS5045 absolute position magnetic rotary encoder ICs.
 - [TLE5012B SPI driver](src/encoders/tle5012b/) - SPI (half duplex) driver for TLE5012B absolute position magnetic rotary encoder IC.
 - [STM32 Hardware Encoder](src/encoders/stm32hwencoder/) - STM32 Hardware timer based encoder driver for ABI type quadrature encoders.
 - [ESP32 Hardware Encoder](src/encoders/esp32hwencoder/) - ESP32 Hardware timer based encoder driver for ABI type quadrature encoders.
 - [SC60228 SPI driver](src/encoders/sc60228/) - SPI driver for SemiMent SC60288 magnetic encoder IC.
 - [MA330 SPI driver](src/encoders/ma330/) - SPI driver for the MPS MagAlpha MA330 absolute position magnetic rotary encoder IC.
 - [MT6816 SPI driver](src/encoders/mt6816/) - SPI driver for the MagnTek MT6816 absolute position magnetic rotary encoder IC.
 - [MT6701 SSI driver](src/encoders/mt6701/) - SSI driver for the MagnTek MT6701 absolute position magnetic rotary encoder IC.
 - [MT6835 SPI driver](src/encoders/mt6835/) - SPI driver for the MagnTek MT6835 21 bit magnetic rotary encoder IC.
 - [STM32 PWM sensor driver](src/encoders/stm32pwmsensor/) - STM32 native timer-based driver for PWM angle sensors.
 - [CalibratedSensor](src/encoders/calibrated/) - A sensor which can calibrate for eccentricity on the magnet placement.
 - [SmoothingSensor](src/encoders/smoothing/) - A SimpleFOC Sensor wrapper implementation which adds angle extrapolation.

### Communications

Extended communications classes for SimpleFOC. In particular the Telemetry and PacketCommander classes, which implement ASCII or Binary communications and allow monitoring and control of multiple motors, via an easy to use "Registers" abstraction. The Binary and ASCII packet based protocols are directly supported in [PySimpleFOC](https://github.com/simplefoc/pysimplefoc).

 - [PacketCommander](src/comms/streams/) - Serial communications with binary protocol, based on register abstraction - get or set any value in SimpleFOC
 - [Telemetry](src/comms/telemetry/) - Telemetry based on registers - monitor any value in SimpleFOC, and log in either ASCII or Binary, compatible with PacketCommander
 - [SimpleTelemetry](src/comms/telemetry/) - Register telemetry for use with Arduino Serial Plotter tool
 - [TeleplotTelemetry](src/comms/telemetry/) - Register telemetry for use with VSCode Teleplot extension
 - [I2CCommander I2C driver](src/comms/i2c/) - I2C communications based on register abstraction
 - [STM32 SpeedDir Input](src/comms/stm32speeddir/) - Control target velocity with PWM speed and direction inputs
 
### Settings

Load and store SimpleFOC motor settings, based on register abstraction. Storing the motor calibration allows you to skip the calibration phase during setup.

 - [SAMD NVM storage driver](src/settings/samd/) - Store settings to either the main flash memory or the RWWEE memory (if available) in your SAMD MCU
 - [CAT24 I2C EEPROM storage driver](src/settings/i2c/) - Store settings to CAT24 I2C EEPROMs
 - [STM32 flash storage driver](src/settings/stm32/) - Store settings directly to STM32 on-board flash, currently supporting STM32G4 MCUs.

### Motor classes

Drive different kinds of motors, or use alternate algorithms to SimpleFOC's default BLDCMotor and StepperMotor classes.

 - [HybridStepperMotor](motors/HybridStepperMotor/) - Drive stepper motors with 3 phases.


### Utilities

Other support code not fitting in the above categories.

 - [STM32 PWM Input driver](src/utilities/stm32pwm/) - PWM Input driver for STM32 MCUs. Accurately measure PWM inputs with zero MCU overhead.
 - [STM32 CORDIC trig driver](src/utilities/stm32math/) - CORDIC driver to accellerate sine and cosine calculations in SimpleFOC, on STM32 MCUs which have a CORDIC unit.
 - [TrapezoidalPlanner](src/utilities/trapezoids/) - Simple trapezoidal motion planner.

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

What's changed since 1.0.6?
- Improvements to LinearHall driver, thanks to dekutree
- Fix for ESP32 compiler warning, thanks to Yannik Stradmann
- Added STM32 LPTIM encoder driver
- Refactored communications code
- Working telemetry abstraction
- Working streams communications, ASCII based
- Refactored settings storage code
- Refactored I2CCommander
- New utility class for simple trapezoidal motion profiles
- Bugfixes [included](https://github.com/simplefoc/Arduino-FOC-drivers/issues?q=milestone%3A1.0.7+)

What's changed since 1.0.5?
- Added STSPIN32G4 driver
- Added STM32G4 CORDIC code, for greatly accellerated trig functions on supported MCUs
- Added STM32FlashSettingsStorage driver, supporting STM32G4 MCUs
- Improvements in the MT6835 sensor driver
- Bugfixes [included](https://github.com/simplefoc/Arduino-FOC-drivers/issues?q=is%3Aissue+milestone%3A1.0.6+)

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
