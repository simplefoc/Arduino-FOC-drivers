
# MT6826S SimpleFOC and SPI driver

!warning not yet tested!
!warning work in progress!

SPI Driver for the MT6826S magnetic position sensor.


## Hardware setup

Connect as per normal for your SPI bus. No special hardware setup is needed to use this driver.

## Software setup

TODO improve documentation and add example to examples folder

Its actually easier to use than the standard SPI sensor class, because it is less generic:

```c++
#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "encoders/mt6826s/MagneticSensorMT6826S.h"

#define SENSOR1_CS 5 // some digital pin that you're using as the nCS pin
MagneticSensorMT6826S sensor1(SENSOR1_CS);


void setup() {
    sensor1.init();
}

unsigned long timestamp = 0;

void loop() {
    sensor.update();
    unsigned long now = millis();
    if (now - timestamp >= 1000) { // print angle every second
        Serial.println(sensor.getAngle());
        timestamp = now;
    }
    delay(10); // update angle every 10ms
}
```


Here's how you can use it:

```c++
    // update the sensor (only needed if using the sensor without a motor)
    sensor1.update();

    // get the angle, in radians, including full rotations
    float a1 = sensor1.getAngle();

    // get the velocity, in rad/s - note: you have to call getAngle() on a regular basis for it to work
    float v1 = sensor1.getVelocity();

    // get the angle, in radians, no full rotations
    float a2 = sensor1.getCurrentAngle();

    // get the raw 15 bit value
    uint16_t raw = sensor1.readRawAngle15();

    // set the zero position
    sensor1.setZeroPosition(sensor1.readRawAngle15()); // set to current position

```

