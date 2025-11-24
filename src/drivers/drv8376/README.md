
# DRV8376 SPI and SimpleFOC driver

DRV8376 is an integrated FET motor driver from TI, capable of 4.5A peak output currents, and operating over the wide range of 4.5V-70V supply. It features 5V and 3.3V LDOs and up to 100kHz PWM frequency. It has internal current sensing and analog current sense outputs for 3 phases.

It is available in SPI and hardware-configured versions. This is a driver for the SPI version.

## Setup

Connect your MCU to the driver as per the datasheets.

You will need at least:

- 3x PWM and 3x EN
- or 6x PWM

And optionally, but recommended:

- DRVOFF
- nSLEEP
- nFAULT
- SOA, SOB, SOC for current sensing
- VREF to set current sensing reference voltage
- ILIM to set current limit

## Software configuration

TODO finish documentation and add an example

```c++
DRV8376Driver6PWM driver = DRV8376Driver6PWM(PIN_AH, PIN_AL, PIN_BH, PIN_BL, PIN_CH, PIN_CL, PIN_nCS, PIN_DRVOFF);


void setup() {
    ...

    driver.init();

    ...
}


```