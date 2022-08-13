# MT6701 SimpleFOC driver

:warning: work in progress... SSI driver is working. I2C not yet complete.

Due to the peculiarities of its interfaces, this very versatile sensor is not directly supported by SimpleFOC's SPI or I2C magnetic sensor implementations. This folder contains dedicated SimpleFOC sensor drivers for the MT6701 for I2C and SSI.

Note: the ABZ, UVW and Analog outputs of this sensor are supported by the standard SimpleFOC encoder, hall-sensor or analog sensor classes respectively.

:warning: Note: the I2C output of this sensor is probably too slow for high performance motor control, but could be useful to program the sensor IC, and to read the absolute angle values for intializing ABZ or UVW modes.

## Hardware setup

For I2C, connect the sensor as normal for I2C, using the SDA and SCL lines. 

:warning: Note: to program the sensor via I2C, it has to be operated at 5V.

For SSI, connect the CSN line to the nCS output of your MCU, the CLK line to the SCLK output of the MCU, and the DO line to the CIPO (MISO) input of the MCU.

In my tests, the sensor was not able to work correctly together with other SPI devices on the same bus, but your experience might differ from mine.

## Software setup

### SSI code sample

```c++
#include "Arduino.h"
#include "SPI.h"
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "encoders/MT6701/MagneticSensorMT6701SSI.h"


#define SENSOR1_CS 5 // some digital pin that you're using as the nCS pin
MagneticSensorMT6701SSI sensor1(SENSOR1_CS);


void setup() {
    sensor1.init();
}
```

To use a custom SPI bus:

```c++
void setup() {
    sensor1.init(&SPI2);
}
```



### I2C code sample

I2C usage is quite simple:

```c++
#include "Arduino.h"
#include "Wire.h"
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "encoders/MT6701/MagneticSensorMT6701I2C.h"

MagneticSensorMT6701I2C sensor1();


void setup() {
    sensor1.init();
}

```

If you've programmed a different I2C address or want to use a different I2C bus you can:

```c++
#define I2C_ADDR 0x70
MagneticSensorMT6701I2C sensor1(I2C_ADDR);


void setup() {
    sensor1.init(&Wire2);
}
```
