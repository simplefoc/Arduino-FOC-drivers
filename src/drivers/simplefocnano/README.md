
# SimpleFOCNano Driver Class

The `SimpleFOCNanoDriver` is a wrapper class around BLDCDriver3PWM to make using the [SimpleFOCNano shield](https://github.com/simplefoc/SimpleFOCNano) super-simple.

If you use this driver you don't need to bother with any pin-numbers, they are all set correctly for you based on the SimpleFOCNano's pinout. Of course, this only works if you actually plug the shield into the Nano. If you use jumper wires, either make exactly the same connections as plugging in the shield would, or don't use this driver.

## Usage

Basic usage is very simple:

```c++
#include <SimpleFOC.h>
#include <SimpleFOCDrivers.h>
#include <drivers/simplefocnano/SimpleFOCNanoDriver.h>

SimpleFOCNanoDriver driver = SimpleFOCNanoDriver();
BLDCMotor motor = BLDCMotor(7);

void setup() {
    driver.voltage_power_supply = driver.getBusVoltage();
    driver.init();
    motor.linkDriver(driver);
    motor.voltage_limit = driver.voltage_limit / 2.0f;
    motor.controller = MotionControlMode::velocity_openloop;
    motor.init();
}

void loop(){
    motor.move(2.0f); // 2 rads per second, open-loop
}
```

### Additional functions

There are some extra features, you can check for faults, and clear the fault state:

```c++
if (driver.isFault()) {
    motor.disable();
    driver.clearFault();
}
```

Or you can activate sleep mode to save power:

```c++
driver.sleep();

// sometime later
if (driver.isSleeping())
    driver.wake();
```

### Bus Voltage Sense

As shown in the example you can read the bus voltage:

:warning: *this is a slow function. Do not call it while motor is running!*

```c++
float val = driver.getBusVoltage(); // get the bus voltage, in Volts
```

### SPI port

The driver's nCS pin is 10, and the constant PIN_nCS can be used:

```c++
MagneticSensorAS5048A sensor = MagneticSensorAS5048A(PIN_nCS);
```

## Examples

The following examples for SimpleFOCNanoDriver can be found in our examples directory:

- [SimpleFOC Nano on AVR](https://github.com/simplefoc/Arduino-FOC-drivers/blob/master/examples/drivers/simplefocnano/simplefocnano_atmega/simplefocnano_atmega.ino) (5V, 10bit ADC range, like original Nano, using AS5048A sensor on SPI)
- [SimpleFOCNanoDriver torque-voltage mode](https://github.com/simplefoc/Arduino-FOC-drivers/blob/master/examples/drivers/simplefocnano/simplefocnano_torque_voltage/simplefocnano_torque_voltage.ino) (3.3V, 12bit ADC range, using AS5048A sensor on SPI)
