
# Current Sense for RP2350

*warning* Work in progress

Implementation of current sensing on RP2350 using the PIO to read SPI based ADCs.


Temporarly, for this to work, we need to add a PWM chanell in Arduino-FOC rp2040 hardware specific files. The duty cyctle needs to be computed to trigger the ADC at the right-time.
The PWM pin is hardcoded to GPIO11 for now.



## Compilling the PIO program
To manually compile the PIO files (generating a .pio.h from a pio), you need to instal pioasm.
Note for self, on my setup:

```
export PIOASM="$HOME/.arduino15/packages/rp2040/tools/pqt-pioasm/4.1.0-1aec55e/pioasm"
"$PIOASM" -v 1 bu79100g_parallel3.pio bu79100g_parallel3.pio.h
```