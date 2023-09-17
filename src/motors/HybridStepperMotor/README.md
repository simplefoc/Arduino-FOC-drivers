# Three Phase / Hybrid Stepper Motor Class

By tying two phases together, you can use a standard bipolar stepper motor with three-phase drivers!
Of course, it requires some different math for commutation. Another limitation is that with SinePWM the motor voltage is limited to 50% of the supply voltage. Using some tricks in SpaceVectorPWM, this max bias can be increased to about 70%.

## Warning

This code has not been tested much! 
It has killed one B-G431B-ESC1 board before. 
However, it has been succesfully used on a few different hardware platforms, so it does work.
BEMF may be the danger here, so take caution with fast decelerations.
Keep in mind that potentially twice the current can be sunk in the shorted phase as the two other legs.

## Hardware setup

Tie two of the phases of the motor together. This is phase "C". The other two phases are A & B. Phase C must go last in the driver constructor. 


## Software setup

Usage is quite simple: 

```c++
#include "Arduino.h"
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "motors/HybridStepperMotor/HybridStepperMotor.h"

HybridStepperMotor motor = HybridStepperMotor(pole_pairs, phase_resistance, kv, winding_inductance);
BLDCDriver3PWM driver = BLDCDriver3PWM(phase_A, phase_B, phase_C, enable);

void setup() {
    driver.init();
    motor.linkDriver(&driver);

    motor.foc_modulation = FOCModulationType::SpaceVectorPWM; //or SinePWM
    motor.init();
    motor.initFOC();
}

void loop(){
    motor.loopFOC();
    motor.move();
}
```