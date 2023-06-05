# SC60228 SimpleFOC driver

Driver for the Semiment 12bit magnetic encoder IC SC60288. According to specs it should support 12 bit accuracy, 10 bit effective accuracy and up to 20kRPM.

Link to [Datasheet](https://semiment.com/wp-content/uploads/2021/04/SC60228_EN-VA1.0.pdf)

The encoder and this driver were tested with a small gimbal motor and MKR1000 (SAMD21 MCU).

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
#include "encoders/sc60228/MagneticSensorSC60228.h"

#define SENSOR1_CS 5 // some digital pin that you're using as the nCS pin
MagneticSensorSC60228 sensor1(SENSOR1_CS);


void setup() {
    sensor1.init();
}
```

Set some options:

```c++
// set counts_per_revolution (which should be the default 4096, but one some hardware appears to be 2048), and spi settings
MagneticSensorSC60228 sensor1(SENSOR1_CS, 4096, mySPISettings);
// or with default SPI settings, just override cpr to a non-default value
MagneticSensorSC60228 sensor1(SENSOR1_CS, 2048);  
```

Use another SPI bus:

```c++
void setup() {
    sensor1.init(SPI2);
}
```
