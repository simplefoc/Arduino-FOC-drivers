# MT6835 SimpleFOC driver

Driver for the MagnTek MT6835 precision magnetic rotary encoder.

This sensor features support for up to 21 bit resolution (!) and speeds up to 120,000RPM. While its full precision requires calibration using an external calibration system, it is impressively precise even in uncalibrated state, and it offers an internal self-calibration mode whose precision is somewhere between the other two.

It has ABZ, UVW and SPI interfaces, and this driver is for its SPI inteface. You can use its ABZ interface with our Encoder classes, but due to the high resolution the interrupt-based Encoder might cause high MCU load. You can use its UVW interface with our Hall Sensor classes.

# Hardware setup

Connect the sensor to an SPI bus on your MCU. Pay attention to the voltage levels needed by your PCBs. The nCS line should have a pull-up resistor.

# Software setup

Usage example:

```c++
#include <Arduino.h>

#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"

#include "encoders/mt6835/MagneticSensorMT6835.h"

#define SENSOR_nCS PB6

SPISettings myMT6835SPISettings(1000000, MT6835_BITORDER, SPI_MODE3);
MagneticSensorMT6835 sensor = MagneticSensorMT6835(SENSOR_nCS, myMT6835SPISettings);

long ts;

void setup() {
    sensor.init();
    ts = millis();
}

void loop() {
    sensor.update();
    long now = millis();
    if (now - ts > 1000) {
        ts = now;
        SimpleFOCDebug::print("A: ");
        SimpleFOCDebug::print(sensor.getAngle());
        SimpleFOCDebug::print(" V: ");
        SimpleFOCDebug::println(sensor.getVelocity());
    }
    delay(10);
}

```

Set the zero position:

```c++
uint16_t pos = sensor.getZeroPosition();    // current value
sensor.setZeroFromCurrentPosition();        // set zero to current position
```

Set the ABZ resolution (needed if you want to use ABZ as the default is 1):

```c++
sensor.setABZResolution(2048);
```

