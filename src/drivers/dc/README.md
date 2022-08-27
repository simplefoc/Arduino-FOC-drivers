# SimpleFOC DCDriver


:warning: code in development! Please help us test it!


**Control DC motors using SimpleFOC's infrastructure.**

SimpleFOC is really intended for field oriented control of PMSM motors, it's in the name ;-). But for different reasons, sometimes DC motors are preferred, and while we're not focused on this use-case, we do have a fair amount of code that can be leveraged to help with DC-Motor control applications.

So this represents a less-supported effort to provide some useful building blocks for DC-Motors.

## DCDriver

This directory contains motor PWM driver code for DC motors. There are different types, represented by different sub-classes, depending on the type of control you want to achieve.

### Common driver features

All the drivers support an optional *enable* pin configured via the class constructor or in the property `driver.pinEN` - if supported, it can be used to switch the driver module on and off.

The polarity of the enable pin can be determined using the property `driver.enable_active_high`

The driver supports a `driver.voltage_limit`, which sets the maximum voltage output, and a `driver.voltage_power_supply` which should be set to the correct value for your application.

If supported by the hardware, the PWM frequency in Hz can be set via `driver.pwm_frequency`.


### DCDriver2PWM

```c++
#define PIN_A 1
#define PIN_B 2
#define PIN_EN 5

DCDriver driver = DCDriver2PWM(PIN_A, PIN_B, PIN_EN);
```

Intended for use with DC motor driver modules or circuits where the two half-bridges of the full-bridge are independently controlled with two input signals, A and B.

To go in one direction, A is pulled high while B is pulled low, and to reverse direction, A is pulled low and B is pulled high.
Speed is controlled by applying PWM to the high signal.

### DCDriverSpeedDir

```c++
#define PIN_PWM 1
#define PIN_DIR 2
#define PIN_EN 5

DCDriver driver = DCDriverSpeedDir(PIN_PWM, PIN_DIR, PIN_EN);
```

Intended for use with DC motor driver modules which accept one speed input, with PWM control, and one direction input (binary logic).

Direction is determined by the DIR pin, while speed is controlled with the PWM signal on the PWM pin.

### DCDriver1PWM

```c++
#define PIN_PWM 1
#define PIN_EN 5
#define THESHOLD 0.5f

DCDriver driver = DCDriver1PWM(PIN_PWM, THESHOLD, PIN_EN);
```

Intended for use when a single PWM signal controls both speed and direction. Typical for RC-Control hardware or continuous rotation servos.

The PWM signal controls the speed and direction by varying the duty cycle. There is a "rest-point" (which can be set via the parameter *threshold*) at which the speed is 0. Duty cycles below the rest-point result in reverse rotation, duty-cycles above the rest-point result in forward rotation.

### DCDriver4PWM

:warning: not yet ready for use!

```c++
#define PIN_A_h 1
#define PIN_A_l 2
#define PIN_B_h 3
#define PIN_B_l 4
#define PIN_EN 5

DCDriver driver = DCDriver4PWM(PIN_A_h, PIN_A_l, PIN_B_h, PIN_B_l, PIN_EN);
```

Intended for use when the motor driver used offers independent control of the four MOSFETs in it's H-Bridge circuit.

The four FETs are switched as follows:

| Control | PIN_A_h | PIN_A_l | PIN_B_h | PIN_B_l |
|:-------:|:-------:|:-------:|:-------:|:-------:|
| Stop    | Off | Off | Off | Off |
| Forward | PWM | Off | Off | On  |
| Reverse | Off | On  | PWM | Off |

