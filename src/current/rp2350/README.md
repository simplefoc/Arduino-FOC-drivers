
# Current Sense for RP2350

*warning* Work in progress

Implementation of current sensing on RP2350 using the PIO to read SPI based ADCs.
Wiring:

* SCK_PIN -> Clock pin shared with 3 ADCs
* CSB_PIN -> Chip Select pin shared with 3 ADCs
* D0, D1 D2 -> The 3 data output from the ADCs. Needs to be contigus (D1=D0+1 D2=D1+1)

Temporarly, for this to work, we need to add a PWM chanell in Arduino-FOC rp2040 hardware specific files. The duty cyctle needs to be computed to trigger the ADC at the right-time.


TODO:
* Find a way to setup the trigger PWM from the driver, or change the arduino-foc lib.
* Select an available PIO and state machine automaticaly.


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
  PhaseCurrent_s phaseCurrents = curr.getPhaseCurrents();
  Serial.printf("%d, %3.3lf, %3.3lf, %3.3lf\r\n", micros(), phaseCurrents.a, phaseCurrents.b, phaseCurrents.c);
}
```


## Compilling the PIO program
To manually compile the PIO files (generating a .pio.h from a pio), you need to instal pioasm.
Note for self, on my setup:

```
export PIOASM="$HOME/.arduino15/packages/rp2040/tools/pqt-pioasm/4.1.0-1aec55e/pioasm"
"$PIOASM" -v 1 bu79100g_parallel3.pio bu79100g_parallel3.pio.h
```

# Internal Design

This library uses:
- One PIO state machine  
- Two DMA channels  
- One PWM (and associated GPIO)


## Trigger Pin

For low-side current measurement, the ADC must sample at the center of the low pulse of the phase PWM signals.  
To achieve this, a *trigger pin* is configured as an auxiliary PWM output, synchronized with the motor PWM but with a fixed duty cycle.  

- This PWM can be mapped to any pin.  
- It does **not** need to be connected to external hardware.  
- The duty cycle is tuned to compensate for the dummy sampling enforced by the BU79100G-LA ADC.  

In the future, this trigger mechanism could also be used to synchronize other types of ADCs.  

**Note (TODO/WIP):** Trigger pin handling is not yet implemented in the driver. Proper synchronization during driver initialization still needs to be added.  


## PIO

The PIO waits for a rising edge on the trigger pin to start two SPI transfers:  
1. A **dummy conversion** (required by the BU79100G-LA when operating at low frequency).  
2. The **actual ADC conversion**.  

Data is captured from 3 SPI data lines in parallel. A 4th line is also sampled but discarded; it can be freely repurposed as a normal GPIO (input or output).  

The PIO writes results to its FIFO as two interleaved 32-bit words in the format: [a1 b1 c1 d1 a2 b2 c2 d2 .... ]

## DMA

Two DMA channels are configured to move data from the PIO FIFO into system memory:  

- **dma_a** transfers incoming 32-bit words into a ring buffer.  
  - The buffer size must be a power of two.  
  - The buffer address must be aligned with its total size.  

- **dma_b** is chained to **dma_a**. It resets the transfer count of **dma_a** by performing a single 32-bit transfer.  
- **dma_a** is then chained back to **dma_b**, creating a self-sustaining loop without CPU intervention.  


## CPU Access to Samples

The CPU can asynchronously access the most recent samples by reading the current DMA write address.  

- If the address is **even**, read samples at positions *n-2* and *n-1*.  
- If the address is **odd**, read samples at positions *n-3* and *n-2*.  
- Address wraparound is handled.

Since the PIO delivers interleaved data, the CPU must de-interleave it. On an RP2350, de-interleaving and copying takes approximately **1 µs** per sample set.  

