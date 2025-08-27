
# Current Sense for RP2350

*warning* Work in progress

Implementation of current sensing on RP2350 using the PIO to read SPI based ADCs.
Wiring:

* SCK_PIN -> Clock pin shared with 3 ADCs
* CSB_PIN -> Chip Select pin shared with 3 ADCs
* D0, D1 D2 -> The 3 data output from the ADCs. Needs to be contigus (D1=D0+1 D2=D1+1)

Temporarly, for this to work, we need to add a PWM chanell in Arduino-FOC rp2040 hardware specific files. The duty cyctle needs to be computed to trigger the ADC at the right-time.


TODO:
* Init a the DMA or IRQ to read samples from PIO.
* Activate the push of adc data in PIO program (disabled for tests)
* Find a way to setup the trigger PWM from the driver, or change the arduino-foc lib.



# Example:
```
// Open loop motor control example With current readings (work in progress)
#include <SimpleFOC.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include <SimpleFOCDrivers.h>
#include "current/rp2350/RP2350PIOCurrentSense.h"

static const uint SCK_PIN   = 2;  // sideset pin
static const uint CSB_PIN   = 3;  // set pin
static const uint D0_PIN    = 4;  // D0..D2 must be contiguous (D1=D0+1, D2=D0+2)
static const uint TRIG_PIN  = 8;  // This need to be also configure as an extra PWM output with custom fixed duty cycle. For now let's just one of the phase as the trigger.

BLDCMotor motor = BLDCMotor(11);
BLDCDriver3PWM driver = BLDCDriver3PWM(7, 8, 9);
RP2350PIOCurrentSense curr = RP2350PIOCurrentSense(1.0, 4096,SCK_PIN, CSB_PIN, D0_PIN, TRIG_PIN);

float target_velocity = 0.2;

void setup() {
  Serial.begin(115200);
  SimpleFOCDebug::enable(&Serial);
  driver.voltage_power_supply = 12;
  driver.voltage_limit = 12;
  driver.init();
  motor.linkDriver(&driver);
  motor.voltage_limit = 2;   // [V]
  motor.controller = MotionControlType::velocity_openloop;
  motor.init();
  Serial.println("Motor ready!");
  _delay(100);
  curr.init();
}

void loop() {
  motor.move(target_velocity);
}
```


## Compilling the PIO program
To manually compile the PIO files (generating a .pio.h from a pio), you need to instal pioasm.
Note for self, on my setup:

```
export PIOASM="$HOME/.arduino15/packages/rp2040/tools/pqt-pioasm/4.1.0-1aec55e/pioasm"
"$PIOASM" -v 1 bu79100g_parallel3.pio bu79100g_parallel3.pio.h
```