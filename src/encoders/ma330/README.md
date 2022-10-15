# MA330 SimpleFOC driver

While MA330 absolute position magnetic rotary encoder is supported by the standard MagneticSensorSPI driver included in the base distribution, this MA330-specific driver includes some optimisations:

- access to the other registers of the MA330
- this driver directly reads the angle with one call to SPI
- this will halve the number of 16-bit SPI transfers per simpleFOC loop iteration


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
#include "encoders/MA330/MagneticSensorMA330.h"

#define SENSOR1_CS 5 // some digital pin that you're using as the nCS pin
MagneticSensorMA330 sensor1(SENSOR1_CS);


void setup() {
    sensor1.init();
}
```

Set some options:

```c++
MagneticSensorMA330 sensor1(SENSOR1_CS, true, mySPISettings);
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

    // get the raw 14 bit value
    uint16_t raw = sensor1.readRawAngle();
```
