# MA735 SimpleFOC driver

This is based on the [MA730](https://github.com/simplefoc/Arduino-FOC-drivers/tree/master/src/encoders/ma730) driver in SimpleFOC, with some tweaks to support the unique registers and options of the MA735. It is a little bit inferior, but less expensive chip after all. The advantage however of this chip (besides cost) is that resolution is inversely proportional to reading speed, ranging from 9-13 bit resolution and 64us to 16ms read times.


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
#include "encoders/ma735/MagneticSensorMA735.h"

#define SENSOR1_CS 5 // some digital pin that you're using as the nCS pin
MagneticSensorMA735 sensor1(SENSOR1_CS);


void setup() {
    sensor1.init();
}
```

Set some options:

```c++
MagneticSensorMA735 sensor1(SENSOR1_CS, mySPISettings);
```

Use another SPI bus:

```c++
void setup() {
    sensor1.init(&SPI2);
}
```

Set some registers before start
```c++
void setup() {
    sensor1.init();
    
    // Note that with this driver there is a write check so registers are not written to if the value is the exact same. Other drivers do not have a write check, and you can easily wear out the NVM every time the code is run. 1,000 cycles max.
    sensor1.setResolution(10.0);
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

    // get the field strength
    FieldStrength fs = sensor1.getFieldStrength();
    Serial.print("Field strength: ");
    Serial.println(fs);

	//get the calculated resolution from FilterWindow register
    float res = sensor.getResolution();
  	Serial1.print("Resolution: ");
  	Serial1.println(res);

	//get the calculated update time in micro seconds from the FilterWindow register
  	int Time = sensor.getUpdateTime();
  	Serial1.print("Update Time (microsecs): ");
  	Serial1.println(Time);
```
