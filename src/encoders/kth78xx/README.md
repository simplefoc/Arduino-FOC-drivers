# KTH7812 SimpleFOC driver

Driver for the KTH7812 magnetic positon sensor.
This driver should also work with other models from the KTH78xx series.

- access the registers of the KTH7812 to read/write settings
- read 16 bit angles
- TODO read 12 bit angles with 4 bit CRC
- supports "fast mode" (return angle with a single 16 bit read)

## Hardware setup

Connect as per normal for your SPI bus. No special hardware setup is needed to use this driver.


## Software setup

Its actually easier to use than the standard SPI sensor class, because it is less generic:

```c++
#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "encoders/kth78xx/MagneticSensorKTH7812.h"

#define SENSOR1_CS 5 // some digital pin that you're using as the nCS pin
MagneticSensorMA330 sensor1(SENSOR1_CS);


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


Use another SPI bus:

```c++
void setup() {
    sensor1.init(SPI2);
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

    // get the raw 16 bit value
    uint16_t raw = sensor1.readRawAngle16();

    // set the ABZ resolution
    sensor1.setABZLimit(1); // 8Mhz ABZ limit
    sensor1.setPPT(1024-1); // set 1024 PPR = 4096 CPR for the ABZ output

```

