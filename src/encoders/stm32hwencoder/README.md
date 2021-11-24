# SimpleFOC Driver for STM32 hardware encoder

This encoder driver uses the STM32 timer hardware to track the A/B impulses, which is much more efficient (and will support higher speeds) than the generic interrupt-based solution used by the standard encoder driver.

Big thank you to @conroy-cheers for originally contributing this code in pull request #114 to the simplefoc repository. Due its hardware-specific nature we moved the code to this drivers repository.

## Warning

This code has not been tested much! Use at your own risk, your milage may vary.
I have tested on: STM32F401CC/TIM4-PB6,PB7

## Hardware setup

The ABI signals of your encoder will have to be connected to the STM32 MCU in such a way that all signals are connected to the same timer. Consult your STM32 chip's datasheet for more information on which pins connect to which timer.

Not all of the timers support the encoder mode, so check the datasheet which timers can be used.

An excellent option can be to use the STM32CubeIDE software's pin assignment view to quickly check which pins connect to which timer.

Note that the index (I) pin is currently not used.


## Software setup

There is no need for interrupt functions or their configuration with this encoder class, so usage is quite simple:

```c++
#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "encoders/stm32hwencoder/STM32HWEncoder.h"


#define ENCODER_PPR 500
#define ENCODER_PIN_A PB6
#define ENCODER_PIN_B PB7
#define ENCODER_PIN_I PC13


STM32HWEncoder encoder = STM32HWEncoder(ENCODER_PPR, ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_PIN_I);


void setup() {
    encoder.init();
}

```