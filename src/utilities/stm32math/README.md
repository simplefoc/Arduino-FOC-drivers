
# SimpleFOC math functions for STM32 CORDIC

Accellerated trigonometry functions are provided for STM32 MCUs having a CORDIC peripheral. The CORDIC hardware can compute sine and cosine values in just a few processor cycles.

## Usage

To use it, set the build-flag `-DHAL_CORDIC_MODULE_ENABLED` for the CORDIC to be enabled in the framework.

Then, include the following in your code:

```c++
#include <Arduino.h>
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "utilities/stm32math/STM32G4CORDICTrigFunctions.h"

void setup() {
    ...

    SimpleFOC_CORDIC_Config();      // initialize the CORDIC

    ...
}

```

That's it. The faster trig functions will be used automatically by the SimpleFOC code.

You can use them yourself in your own code too:

```c++

float angle = 0.5f; // in radians
float s = _sin(angle);
float c = _cos(angle);

// get both sine and cosine in one operation
_sincos(angle, &s, &c);

```