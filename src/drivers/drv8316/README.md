
# DRV8316 SimpleFOC driver

The DRV8316 is a integrated FET, integrated current sensing 3-phase BLDC driver IC from Texas Instruments, including all protections and many cool configuration optons. See https://www.ti.com/product/DRV8316 for more information.

This driver includes a DRV8316 SPI driver, and specific subclasses for SimpleFOCs BLDCDriver3PWM and BLDCDriver6PWM generic drivers. The code is designed to be "Ardunio compatible" and should work with any of the hardware architectures supported by SimpleFOC.

## Hardware setup

To use the DRV8316 you will have to connect the following:

- GND
- SPI MOSI
- SPI MISO
- SPI CLK
- SPI nCS
- INH_U - connect to motor PWM pin
- INH_V - connect to motor PWM pin
- INH_W - connect to motor PWM pin
- INL_U - connect to motor PWM pin for 6-PWM, or to digital out (or pull up to VCC) for 3-PWM operation
- INL_V - connect to motor PWM pin for 6-PWM, or to digital out (or pull up to VCC) for 3-PWM operation
- INL_W - connect to motor PWM pin for 6-PWM, or to digital out (or pull up to VCC) for 3-PWM operation

Optionally, but probably useful:

- nFault - digital in, active low
- DRVOFF - digital out

For current sensting:

- VREF - either connect to DAC output of the MCU or a fixed voltage reference.
- ISENA - connect to analog in
- ISENB - connect to analog in
- ISENC - connect to analog in

For current limiting:

- ILIM (same as VREF) - connect it to a DAC output of the MCU if you want to control the current limit from the MCU

## Usage

Usage is quite easy, especially if you already know SimpleFOC. See also the [examples](https://github.com/simplefoc/Arduino-FOC-drivers/examples/drivers/drv8316/)

```c++
#include "Arduino.h"
#include <Wire.h>
#include <SimpleFOC.h>
#include <Math.h>
#include "SimpleFOCDrivers.h"
#include "drivers/drv8316/drv8316.h"

BLDCMotor motor = BLDCMotor(11);
DRV8316Driver6PWM driver = DRV8316Driver6PWM(A3,0,A4,1,2,6,7,false);          // MKR1010 6-PWM

//... normal simpleFOC init code...
```

Or, for 3-PWM:

```c++
#include "Arduino.h"
#include <Wire.h>
#include <SimpleFOC.h>
#include <Math.h>
#include "SimpleFOCDrivers.h"
#include "drivers/drv8316/drv8316.h"

BLDCMotor motor = BLDCMotor(11);
DRV8316Driver3PWM driver = DRV8316Driver3PWM(A3,A4,2,7,false);          	  // MKR1010 3-PWM
// these are examples, for 3-PWM you could use any output pins as the enable pins.
#define ENABLE_A 0
#define ENABLE_B 1
#define ENABLE_C 6

void setup() {


	pinMode(ENABLE_A, OUTPUT);
	digitalWrite(ENABLE_A, 1); // enable
	pinMode(ENABLE_B, OUTPUT);
	digitalWrite(ENABLE_B, 1); // enable
	pinMode(ENABLE_C, OUTPUT);
	digitalWrite(ENABLE_C, 1); // enable


//... normal simpleFOC init code...


}

```

You can use the driver's features. In general you can do this at any time, but certain features only make sense at setup-time (e.g. setting the PWM mode, which is handled automatically by the DRV8316Driver3PWM class, or setting the current limit, which you generally want to get done before applying power to the motor).

Driver usage, for example reading and printing the complete status:

```c++
	DRV8316Status status = driver.getStatus();
	Serial.println("DRV8316 Status:");
	Serial.print("Fault: ");
	Serial.println(status.isFault());
	Serial.print("Buck Error: ");
	Serial.print(status.isBuckError());
	Serial.print("  Undervoltage: ");
	Serial.print(status.isBuckUnderVoltage());
	Serial.print("  OverCurrent: ");
	Serial.println(status.isBuckOverCurrent());
	Serial.print("Charge Pump UnderVoltage: ");
	Serial.println(status.isChargePumpUnderVoltage());
	Serial.print("OTP Error: ");
	Serial.println(status.isOneTimeProgrammingError());
	Serial.print("OverCurrent: ");
	Serial.print(status.isOverCurrent());
	Serial.print("  Ah: ");
	Serial.print(status.isOverCurrent_Ah());
	Serial.print("  Al: ");
	Serial.print(status.isOverCurrent_Al());
	Serial.print("  Bh: ");
	Serial.print(status.isOverCurrent_Bh());
	Serial.print("  Bl: ");
	Serial.print(status.isOverCurrent_Bl());
	Serial.print("  Ch: ");
	Serial.print(status.isOverCurrent_Ch());
	Serial.print("  Cl: ");
	Serial.println(status.isOverCurrent_Cl());
	Serial.print("OverTemperature: ");
	Serial.print(status.isOverTemperature());
	Serial.print("  Shutdown: ");
	Serial.print(status.isOverTemperatureShutdown());
	Serial.print("  Warning: ");
	Serial.println(status.isOverTemperatureWarning());
	Serial.print("OverVoltage: ");
	Serial.println(status.isOverVoltage());
	Serial.print("PowerOnReset: ");
	Serial.println(status.isPowerOnReset());
	Serial.print("SPI Error: ");
	Serial.print(status.isSPIError());
	Serial.print("  Address: ");
	Serial.print(status.isSPIAddressError());
	Serial.print("  Clock: ");
	Serial.print(status.isSPIClockFramingError());
	Serial.print("  Parity: ");
	Serial.println(status.isSPIParityError());
	if (status.isFault())
		driver.clearFault();
	delayMicroseconds(1); // ensure 400ns delay
	DRV8316_PWMMode val = driver.getPWMMode();
	Serial.print("PWM Mode: ");
	Serial.println(val);
	delayMicroseconds(1); // ensure 400ns delay
	bool lock = driver.isRegistersLocked();
	Serial.print("Lock: ");
	Serial.println(lock);
```


Setting options can be conveniently done via the provided setter methods. All documented registers and options are available via the driver, and the option values can be accessed via enums.

Leave at least 400ns delay between reading and/or writing options to ensure you don't talk to the DRV8316 too quickly:

```c++
driver.setPWM100Frequency(DRV8316_PWM100DUTY::FREQ_40KHz);
delayMicroseconds(1); // ensure 400ns delay
driver.setBuckVoltage(DRV8316_BuckVoltage::VB_5V7);
```


### Current sensing

TODO...

### Current limiting

TODO...

