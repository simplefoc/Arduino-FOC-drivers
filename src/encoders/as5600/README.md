# AS5600 SimpleFOC driver

I2C protocol driver for the AMS AS5600 magnetic encoder (digital potentiometer). Also supports the newer AS5600L variant.

:warning: work in progress

## Hardware setup

Connect the sensor to 3.3V or 5V power as appropriate, and to I2C (SDA and SCL).

Important: please make sure the direction pin (DIR) is either pulled up to VDD, or down to GND. Do not leave the direction pin floating.

## Software setup

The sensor driver is easy to use.

```c++
#include <Arduino.h>
#include <SimpleFOC.h>
#include <SimpleFOCDrivers.h>
#include "encoders/as5600/MagneticSensorAS5600.h"

MagneticSensorAS5600 sensor;

void setup() {
    sensor.init();
}

long ts;

void loop() {
  sensor.update();
  if (millis() - ts > 1000) {
    Serial.println(sensor.getAngle(), 3);
    ts = millis();
  }
  delay(1);
}
```

You can use a different I2C bus by passing a pointer to its TwoWire object in the init method:

```c++
MagneticSensorAS5600 sensor1;
MagneticSensorAS5600 sensor2;
TwoWire myI2C;

void setup() {

    ...

    sensor1.init(&myI2C);
    sensor2.init(&Wire2);
}
```

Using the sensor without releasing the bus should give you considerably more speed. Setting closeTransactions to false means the sensor will not release the bus between angle reads, and also will not re-write the register to the device, which boosts angle reading throughput significantly.

```c++
MagneticSensorAS5600 sensor;

void setup() {
    sensor.closeTransactions = false;
    sensor.init();
}
```

The sensor's other registers are exposed. Note that using the setter functions to set register values only performs a normal write, not a permanent programing of the register. Permanent programming (BURN function) is not supported by this driver.

```c++

MagneticSensorAS5600 sensor;

void setup() {
    sensor.closeTransactions = false;
    sensor.init();

    uint16_t magnitude = sensor.readMagnitude();
    AS5600Status status = sensor.readStatus();
    // print the values or something
}

```

The AS5600L has a default address of 0x40, and you can set the I2C address. To temporarily change the address (resets to default on restart):

```c++

MagneticSensorAS5600 sensor(0x40); // default address of AS5600L is 0x40

void setup() {
    Serial.begin(115200);
    delay(2000);

    sensor.closeTransactions = true; // use normal transactons
    sensor.init();
    sensor.setI2CAddr(0x41); // set address to 0x41
    sensor.setI2CUpdt(0x41); // i2c address is now 0x41
    sensor._address = 0x41;
    delay(10);
    // sensor is using new address
    uint16_t magnitude = sensor.readMagnitude();
}
```


You can program an I2C address permanently (AS5600L only). The address will remain after reset. This programming operation can only be done once.

```c++

MagneticSensorAS5600 sensor(0x40); // default address of AS5600L is 0x40

void setup() {
    Serial.begin(115200);
    delay(2000);

    sensor.closeTransactions = true; // use normal transactons
    sensor.init();

    uint8_t addr = sensor.readI2CAddr(); // read I2C address, should be 0x40
    Serial.print("Current Address: ");
    Serial.println(addr);

    sensor.setI2CAddr(0x41); // set address to 0x41
    sensor.burnSettings(); // permanently set new address
    Serial.println("Permanently changed address to 0x41. Restart the system.");
    while (1);
}


```