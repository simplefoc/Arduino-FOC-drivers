# MT6835 SimpleFOC driver

:warning: untested

Driver for the MagnTek MT6835 precision magnetic rotary encoder.

This sensor features support for up to 21 bit resolution (!) and speeds up to 120,000RPM. While its full precision requires calibration using an external calibration system, it is impressively precise even in uncalibrated state, and it offers an internal self-calibration mode whose precision is somewhere between the other two.

It has ABZ, UVW and SPI interfaces, and this driver is for its SPI inteface. You can use its ABZ interface with our Encoder classes, but due to the high resolution the interrupt-based Encoder might cause high MCU load. You can use its UVW interface with our Hall Sensor classes.

# Hardware setup

Connect the sensor to an SPI bus on your MCU. Pay attention to the voltage levels needed by your PCBs. The nCS line should have a pull-up resistor.

# Software setup

Usage example:

```c++
#include "Arduino.h"
#include "Wire.h"
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "encoders/MT6835/MagneticSensorMT6835.h"

// TODO code example
```

