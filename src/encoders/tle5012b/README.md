# SimpleFOC Driver for TLE5012B encoder for STM32 MCUs

This driver code is compatible with STM32 MCUs. Based loosely on code from [here](https://github.com/xerootg/btt-s42b-simplefoc), with many thanks to @xerootg

## Hardware setup

Connect the data pin of the sensor to the MOSI (COPI) line of the MCU. The SCLK and nCS lines are connected as normal.

## Software setup

Sample code:

```
#include <Arduino.h>
#include <SimpleFOC.h>
#include <SimpleFOCDrivers.h>
#include "encoders/tle5012b/MagneticSensorTLE5012B.h"

MagneticSensorTLE5012B sensor(PB15,PB13,PB12);

long ts;

void setup() {
  Serial.begin(115200);
  while (!Serial) ;
  delay(2000);

  Serial.println("Initializing sensor...");

  sensor.init();

  Serial.println("Sensor initialized.");

  ts = millis();
}

void loop() {
  sensor.update();
  if (millis() - ts > 1000) {
    Serial.println(sensor.getAngle(), 3);
    ts = millis();
  }
  delay(1);
}
```

## Other MCUs

Infineon provides a driver library compatible with Atmel AVR (Arduino UNO, Nano, etc...) and Infineon XMC MCUs. If you have one of those MCUs, you can use this library in conjunction with the GenericSensor class in SimpleFOC.

```
#include <SimpleFOC.h>
#include <TLE5012-ino.hpp>

Tle5012Ino Tle5012Sensor = Tle5012Ino();
errorTypes checkError = NO_ERROR;

float readMySensor(){
 // read my sensor
 // return the angle value in radians in between 0 and 2PI
 
    Tle5012Sensor.getAngleValue(d);
    d = (d + 180);
    if ( d + 40.9 > 360 ) {
        d = (d + 40.9) - 360;
    }
    else
        d = d + 40.9;

    d = d * 0.0174533;
    return d;
}
void initMySensor(){
  // do the init
   checkError = Tle5012Sensor.begin();
}

// empty constructor 
GenericSensor sensor = GenericSensor(readMySensor, initMySensor);

void setup(){
...
  //init sensor and use it further with foc...
  sensor.init();
...
}
```

Other MCUs are currently not supported. The problem is that the sensor uses SPI in half-duplex mode, which is not supported by Arduino framework. So for each MCU type a custom driver has to be written to get the half-duplex SPI communication to work. There are currently no plans to support other MCUs on our side, but we will gladly accept pull requests :-)

If you can find other libraries for other MCUs, you can use the GenericSensor approach to integrate them.

## Safety word validation

The SSC protocol appends a safety word after the data on every read, carrying
four active-low status bits and a CRC over the transaction. This driver already
clocks those bytes off the wire, so validating them costs no extra bus traffic.

It is disabled by default, leaving existing behaviour unchanged. Enable it when
a corrupted angle would be acted on — for example once the sensor is closing a
position loop and a bad reading commands current:

```cpp
sensor.checkSafetyWord(true);
```

With it enabled, a read whose CRC fails or whose status bits report a fault is
rejected and the previous good angle is returned instead. Two accessors expose
what happened:

```cpp
sensor.safetyErrorCount();   // reads rejected since init
sensor.lastSafetyWord();     // raw safety word from the most recent read
```

The CRC is CRC-8, polynomial `0x1D`, seed `0xFF`, computed over the transmitted
command word followed by the returned data word, with the result inverted.
