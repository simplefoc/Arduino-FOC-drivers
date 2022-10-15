# SimpleFOC Driver and Support Library

![Library Compile](https://github.com/simplefoc/Arduino-FOC-drivers/workflows/Library%20Compile/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![Release](https://img.shields.io/badge/version-1.0.2-blue)

This library contains an assortment of drivers and supporting code for SimpleFOC.

The intent is to keep the core of SimpleFOC clean, and thus easy to maintain, understand and port to different platforms. In addition to this core, there are various drivers and supporting code which has grown around SimpleFOC, and which we would like to make available to the community.

## New Release

v1.0.2 - Released Oct 2022, for Simple FOC 2.2.3

What's changed since 1.0.1?
- Calibrated sensor by @MarethyuPrefect
- New Sensors: MT6701, MA330, MT6816
- Fixes bugs


## What is included

What is here? See the sections below. Each driver or function should come with its own more detailed README.

### Motor/Gate driver ICs

 - [DRV8316 driver](src/drivers/drv8316/) - SPI driver for TI's DRV8316 motor driver IC.

### Encoders

 - [AS5048A SPI driver](src/encoders/as5048a/) - SPI driver for the AMS AS5048A absolute position magnetic rotary encoder IC.
 - [AS5047 SPI driver](src/encoders/as5047/) - SPI driver for the AMS AS5047P and AS5047D absolute position magnetic rotary encoder ICs.
 - [AS5047U SPI driver](src/encoders/as5047u/) - SPI driver for the AMS AS5047U absolute position magnetic rotary encoder ICs.
 - [MA730 SPI driver](src/encoders/ma730/) - SPI driver for the MPS MagAlpha MA730 absolute position magnetic rotary encoder IC.
 - [MA730 SSI driver](src/encoders/ma730/) - SSI driver for the MPS MagAlpha MA730 absolute position magnetic rotary encoder IC.
 - [AS5145 SSI driver](src/encoders/as5145/) - SSI driver for the AMS AS5145 and AS5045 absolute position magnetic rotary encoder ICs.
 - [TLE5012B SPI driver](src/encoders/tle5012b/) - SPI (half duplex) driver for TLE5012B absolute position magnetic rotary encoder IC.
 - [STM32 Hardware Encoder](src/encoders/stm32hwencoder/) - Hardware timer based encoder driver for ABI type quadrature encoders.
 - [SC60228 SPI driver](src/encoders/sc60228/) - SPI driver for SemiMent SC60288 magnetic encoder IC.
 - [MA330 SPI driver](src/encoders/ma330/) - SPI driver for the MPS MagAlpha MA330 absolute position magnetic rotary encoder IC.
 - [MT6816 SPI driver](src/encoders/mt6816/) - SPI driver for the MagnTek MT6816 absolute position magnetic rotary encoder IC.
 - [MT6701 SSI driver](src/encoders/mt6701/) - SSI driver for the MagnTek MT6701 absolute position magnetic rotary encoder IC.

### Communications

 - [I2CCommander I2C driver](src/comms/i2c/) - I2C communications protocol and drivers for both controller and target devices. 


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
