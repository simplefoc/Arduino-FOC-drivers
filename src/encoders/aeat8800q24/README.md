# AEAT-8800-Q24 SimpleFOC driver

SPI/SSI driver for the absolute position magnetic rotary encoder. This encoder is not supported by the
normal SimpleFOC drivers due to its mixed SPI/SSI mode.

- access to the configuration registers of the AEAT-8800-Q24, enabling you to set parameters
- angles are read via SSI, with 16 bit (!) precision
- currently only 16 bit resolution is supported, don't lower the resolution


## Hardware setup

Connect as per normal for your SPI bus. No special hardware setup is needed to use this driver. nCS pin is
required.

Note that due to the way SSI and SPI share pins, you can normally only run one of these sensors per SPI bus.

## Software setup

Its actually easier to use than the standard SPI sensor class, because it is less generic:

```c++
#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "encoders/aeat8800q24/MagneticSensorAEAT8800Q24.h"

#define SENSOR1_CS 5 // some digital pin that you're using as the nCS pin
MagneticSensorAEAT8800Q24 sensor1(SENSOR1_CS);


void setup() {
    sensor1.init();
}
```

Set some options:

```c++
MagneticSensorAEAT8800Q24 sensor1(SENSOR1_CS, MOSI_pin, mySPISettings, mySSISettings);
```

Use another SPI bus:

```c++
void setup() {
    sensor1.init(SPI2);
}
```

Here's how you can use it:

```c++
    // get the angle, in radians, including full rotations
    float a1 = sensor1.getAngle();

    // get the velocity, in rad/s - note: you have to call getAngle() on a regular basis for it to work
    float v1 = sensor1.getVelocity();

    // get the angle, in radians, no full rotations
    float a2 = sensor1.getCurrentAngle();

    // get the raw 16 bit value
    uint16_t raw = sensor1.readRawAngle();

    // check status
    float angle = sensor1.getSensorAngle();
    AEAT8800Q24_Status_t status = sensor1.getLastStatus();
    if (status.mlo)
        Serial.println("Sensor magnet low error");
    if (status.mhi)
        Serial.println("Sensor magnet high error");
```
