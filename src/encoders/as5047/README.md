# AS5047 SimpleFOC driver

While the AS5047 absolute position magnetic rotary encoder is supported by the standard MagneticSensorSPI driver included in the base distribution, this AS5047-specific driver includes some optimisations:

- access to the other registers of the AS5047, including the magnitude value which can be used to check the magnet strength, and the diagnostics register
- access to the error state of the sensor, and ability to clear errors
- it has a fastMode setting, in which the sensor is sent only 1 command per getAngle() call - the value returned will be from previous getAngle() invocation

This driver should work with AS5047P and AS5047D models. The AS5047U has it's own driver [here](../as5047u/).

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
#include "encoders/as5047/MagneticSensorAS5047.h"

#define SENSOR1_CS 5 // some digital pin that you're using as the nCS pin
MagneticSensorAS5047 sensor1(SENSOR1_CS);


void setup() {
    sensor1.init();
}
```

Set some options:

```c++
MagneticSensorAS5047 sensor1(SENSOR1_CS, true, mySPISettings);
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

    // read the CORDIC magnitude value, a measure of the magnet field strength
    float m1 = sensor1.readMagnitude();

    // check for errors
    if (sensor1.isErrorFlag()) {
        AS5047Error error = sensor1.clearErrorFlag();
        if (error.parityError) { // also error.framingError, error.commandInvalid
            // etc...            
        }
    }

    // get diagnostics
    AS5047Diagnostics diagnostics = sensor1.readDiagnostics();
```

