
# TMC6200 SimpleFOC Driver

by [@YaseenTwati](https://github.com/YaseenTwati)

The TMC6200 is a Universal high voltage BLDC/PMSM/Servo MOSFET 3-halfbridge gate-driver with in line motor current
sensing. External MOSFETs for up to 100A motor current.

See https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC6200_datasheet_Rev1.05.pdf for more information.

## Hardware setup

To use the TMC6200 you will have to connect the following:

- GND
- VCC_IO - 3.3V or 5V
- SPE - pull up to VCC to enable SPI mode
- SPI MOSI ( SDI )
- SPI MISO ( SDO )
- SPI CLK
- SPI nCS
- DRV_EN - connect to digital out (or pull up to VCC)
- UH - connect to motor PWM pin
- VH - connect to motor PWM pin
- WH - connect to motor PWM pin
- UL - connect to motor PWM pin for 6-PWM, or to digital out (or pull up to VCC) for 3-PWM operation
- VL - connect to motor PWM pin for 6-PWM, or to digital out (or pull up to VCC) for 3-PWM operation
- WL - connect to motor PWM pin for 6-PWM, or to digital out (or pull up to VCC) for 3-PWM operation

Optionally, but useful:

- FAULT - digital in, active high

For current sensing:

- CURU - connect to analog in
- CURV - connect to analog in
- CURW - connect to analog in

## Usage

Usage is quite easy, especially if you already know SimpleFOC. See also the [examples](https://github.com/simplefoc/Arduino-FOC-drivers/examples/drivers/drv8316/)

```c++
#include "Arduino.h"
#include <SimpleFOC.h>
#include "SimpleFOCDrivers.h"
#include "drivers/tmc6200/TMC6200.hpp"

BLDCMotor motor = BLDCMotor(15);
TMC6200Driver6PWM driver = DRV8316Driver6PWM(UH, UL, VH, VL, WH, WL, nCS, DRV_EN);

//... normal simpleFOC init code...
```

Or, for 3-PWM:

```c++
#include "Arduino.h"
#include <SimpleFOC.h>
#include "SimpleFOCDrivers.h"
#include "drivers/tmc6200/TMC6200.hpp"

BLDCMotor motor = BLDCMotor(15);
TMC6200Driver3PWM driver = TMC6200Driver3PWM(UH, MOTOR_VH, MOTOR_WH, nCS, DRV_EN);

void setup() {
    
    pinMode(UL, OUTPUT);
    pinMode(VL, OUTPUT);
    pinMode(WL, OUTPUT);
    
    digitalWrite(WL, HIGH);
    digitalWrite(UL, HIGH);
    digitalWrite(VL, HIGH);

    //... normal simpleFOC init code...
}

```

### Validating the SPI Connection
You can validate the SPI connection by checking the value of VERSION field in IOIN register. The value should be 0x10.

```c++
    if(driver.getInputs().VERSION != TMC6200_VERSION){
        // something is wrong with the spi connection
    }
```

### Current Sensing
The gain of the internal current amplifiers can be set to 5, 10 or 20 through `setCurrentSenseGain()`

```c++
    driver.setCurrentSenseGain(TMC6200_AmplificationGain::_5);
    //driver.setCurrentSenseGain(TMC6200_AmplificationGain::_10);
    //driver.setCurrentSenseGain(TMC6200_AmplificationGain::_20);
```
The sense amplifiers can also be turned off ( they are on by default ), through `setCurrentSenseAmplifierState()`

```c++
    driver.setCurrentSenseAmplifierState(false);
```
### Driver Strength
The strength of the mosfet drivers can be controlled through `setDriverStrength()`

```c++
    driver.setDriverStrength(TMC6200_DRVStrength::Weak);
    //driver.setDriverStrength(TMC6200_DRVStrength::WeakTC); // (medium above OTPW level)
    //driver.setDriverStrength(TMC6200_DRVStrength::Medium);
    //driver.setDriverStrength(TMC6200_DRVStrength::Strong);
```

### Handling Faults
The fault line will go high if a fault occurs such as a short, an interrupt can be used to handle it.
Note that some faults will disable the driver and will require the DRV_EN to be cycled to clear the fault.
    
```c++
    // somewhere in setup
    attachInterrupt(digitalPinToInterrupt(FAULT), handleFault, RISING);
```

```c++
    void handleFault()
    {
        // you can read the status register to see what happened
        TMC6200GStatus status = driver.getStatus();
        Serial.print("hasUShorted: "); Serial.println(status.hasUShorted());
        Serial.print("hasVShorted: "); Serial.println(status.hasVShorted());
        Serial.print("hasWShorted: "); Serial.println(status.hasWShorted());
        Serial.print("isUShortedToGround: "); Serial.println(status.isUShortedToGround());
        Serial.print("isUShortedToSupply: "); Serial.println(status.isUShortedToSupply());
        Serial.print("isVShortedToGround: "); Serial.println(status.isVShortedToGround());
        Serial.print("isVShortedToSupply: "); Serial.println(status.isVShortedToSupply());
        Serial.print("isWShortedToGround: "); Serial.println(status.isWShortedToGround());
        Serial.print("isWShortedToSupply: "); Serial.println(status.isWShortedToSupply());
        Serial.print("isOverTemperaturePreWarning: "); Serial.println(status.isOverTemperaturePreWarning());
        Serial.print("isChargePumpUnderVoltage: "); Serial.println(status.isChargePumpUnderVoltage());
        
        // the driver must be cycled to clear the fault
        digitalWrite(DRV_EN, LOW);
        delayMicrosockets(someSmallDelay);
        digitalWrite(DRV_EN, HIGH);
    }
```

The driver provides other features such as controlling the tolerences of short detection and the BBM cycle time and so on, setting the options can be conveniently done via the provided setter methods. All documented registers and options are available via the driver, and the option values can be accessed via enums.
