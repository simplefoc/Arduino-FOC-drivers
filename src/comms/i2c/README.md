
# I2CCommander

Implementation of SimpleFOC Commander for I2C communication bus.

This code takes the point of view that the motor driver (the "muscle") is the I2C target device, and another MCU/CPU (the "brain") is the I2C controller device, which is coordinating one or more motor drivers on the same or different I2C buses. Each motor driver can offer one or more motors for control via the I2CCommander. So fairly flexible setups are possible, with multiple motors per driver, multiple drivers per I2C bus and multiple I2C buses on the brain MCU.

## Warning

This is new code, and has not been extensively tested. Your milage may vary. That said, basic use cases have been tested, and we would certainly appreciate feedback and help with testing it out.

In particular, there are concurrency issues with reading/writing the SimpleFOC motor values from I2C while the motor is running. These should be solved soon in an upcoming version.

**Do not run on 8-bit MCUs!** The code currently assumes atomic 32 bit reads, so running on Arduino UNO or Nano is unfortunately a no-go.

## Using

As would be expected for I2C, each target device needs a unique I2C address on its bus, and setting up and discovering these addresses is out-of-scope for I2CCommander. Setting up and configuring the TwoWire objects (which pins, speed, etc...) is also out of scope and finished, initialized TwoWire objects must be passed to I2CCommander. If you don't specify a different reference, the standard *Wire* object is assumed.

Communication with the motor drivers happens via a register paradigm. The driver board offers many registers, some of which can be read, some can be written, and some are read/write. The controller MCU sends I2C messages to the target device to read or write a register as desired. The size of the data to be read/written depends on the register, and must be known by the controller. See Registers, below, for more details on the individual registers.

Since each target motor driver can handle multiple motors, one of the registers contains the currently selected motor. Most of the other registers then operate on the currently selected motor. There are some exceptions, like REG_ENABLE_ALL - which operates on all the motors, or REG_STATUS, which returns stati for all the motors.

### Target device (motor driver)

The target device (motor driver) initializes and uses an instance of I2CCommander. Only one instance is needed for all attached motors:

```c++
#include "Arduino.h"
#include <Wire.h>
#include <SimpleFOC.h>
#include "SimpleFOCDrivers.h"
#include "comms/i2c/I2CCommander.h"

// commander instance
uint8_t i2c_addr = 0x60;  // can be anything you choose
I2CCommander commander;
// interrupt callbacks
void onReceive(int numBytes) { commander.onReceive(numBytes); }
void onRequest() { commander.onRequest(); }


// ... other variables, like sensor, etc...
BLDCMotor motor = BLDCMotor(POLE_PAIRS);


void setup() {
    
    // ...other setup code

    Wire.setClock(400000);          // use same speed on controller device
    Wire.begin(i2c_addr, true);     // initialize i2c in target mode
    commander.addMotor(&motor);     // add a motor
    //commander.addMotor(&motor2);  // you could add more than one motor
    commander.init(i2c_addr);       // initialize commander
    Wire.onReceive(onReceive);      // connect the interrupt handlers
    Wire.onRequest(onRequest);

}
```

### Controller device ("brain" MCU)

On the controller device, you use an instance of I2CCommanderMaster, which you initialize by adding one or more target devices to it:

```c++
#include "Arduino.h"
#include <Wire.h>
#include <SimpleFOC.h>
#include "SimpleFOCDrivers.h"
#include "comms/i2c/I2CCommanderMaster.h"

#define TARGET_I2C_ADDRESS 0x60
I2CCommanderMaster commander;

void setup() {
    
    // ...other setup code

    Wire.setClock(400000);          // use same speed on target device!
    Wire.begin();                   // initialize i2c in controller mode
    commander.addI2CMotors(TARGET_I2C_ADDRESS, 1);            // add target device, it has 1 motor
    //commander.addI2CMotors(TARGET_I2C_ADDRESS2, 1);         // you could add another target device on the same bus
    //commander.addI2CMotors(TARGET_I2C_ADDRESS, 1, &wire2);  // or on a different i2c bus
    commander.init();               // init commander
}

```


## Extending

The API is somewhat opinionated, and unlike the standard serial commander, currently does not support hooking in your own variables for reading/writing to them via I2C. This is because I2C is a bit less flexible than Serial.

If you want to extend I2CCommander please subclass it and override the functions `sendRegister` and `receiveRegister` to add new registers. Use register numbers above 0x80 to prevent collisions with the standard registers.