
# SimpleFOC STSPIN32G4 Driver

This driver initializes the PWM stage of the STSPIN32G4, and provides access to its configuration via I2C.

:warning: in development!

## Setup

Since there are currently no standard boards for Arduino based on the STSPIN32G4 you will need a custom board definition, associated linker script and project setup to compile for your board. These topics are out of scope for this driver, but you can find a working example for the [FunQi STSPIN32G4 board](TODO link) [here](TODO link);

Once you can compile for your board, and flash it with a "blinky" test sketch, then you're ready to try SimpleFOC and more complex code.

## Usage

Basic usage, as you can see it is very simple. Since the pins are all pre-defined due to internal connections, setup
is easier than with the standard drivers. Here is an example for open loop mode:

```c++
#include <Arduino.h>
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "drivers/stspin32g4/STSPIN32G4.h"


STSPIN32G4 driver = STSPIN32G4();
BLDCMotor motor = BLDCMotor(7);

void setup() {
    driver.voltage_power_supply = 12.0f;
    driver.init();
    motor.voltage_limit = driver.voltage_limit / 2.0f;
    motor.controller = MotionControlType::velocity_openloop;
    motor.linkDriver(&driver);
    motor.init();
}

void loop(){
    motor.move(5.0f); // 5 rad/s open loop
    delayMicroseconds(100); // STM32G4 is very fast, add a delay in open loop if we do nothing else
}
```