# AS5145 SimpleFOC driver

SSI protocol driver for the AMS AS5145 magnetic encoder. Any of the A, B or H variants should work. AS5045 encoders should also be supported.

Only angle reading is supported, might get to the status bits at a later time.
The SSI protocol is "emulated" using the SPI peripheral.

Tested with AS5145A on STM32G491 so far.

## Hardware setup

Wire the sensor's data (DO) line to the MISO (CIPO) pin, nCS, SCK as normal. Leave the MOSI pin unconnected.

## Software setup

```
#include <Arduino.h>
#include <SimpleFOC.h>
#include <SimpleFOCDrivers.h>
#include "encoders/as5145/MagneticSensorAS5145.h"

MagneticSensorAS5145 sensor;
SPIClass spi_ssi(PB15, PB14, PB13, PB12);

long ts;

void setup() {
  Serial.begin(115200);
  while (!Serial) ;
  delay(2000);

  Serial.println("Initializing sensor...");

  spi_ssi.begin();
  sensor.init(&spi_ssi);

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