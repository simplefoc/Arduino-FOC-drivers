
# STM32MagneticSensorPWM

STM32 MCU specific PWM sensor class. This class uses the STM32's hardware timers to precisely capture the PWM input signal, and doesn't use interrupts or have MCU overhead.

:warning: this code is not yet well tested.

## Setup

Please use an advanced control or general purpose timer pin on your STM32. Connect the sensor's PWM output to the MCU's input pin. Usually, pull-ups or pull-downs are not needed, but check your sensor's datasheet.

:warning: only tested on 16 bit timers. Code changes may be needed to make it work on 32 but timers. Avoid using TIM2 and TIM5 unless you want to test it.

The sensor needs the values `min_ticks` and `max-ticks` to be configured correctly to convert the PWM input into an angle. These values will depend on the sensor, but also on the MCU's timer clock speed.

To print the current tick value, use:

```
sensor.getDutyCycleTicks();
```

By rotating the motor through several full turns while printing the ticks to the screen you will be able to determine the correct values empirically.

## Usage

```
STM32MagneticSensorPWM sensor = STM32MagneticSensorPWM(PB7, 412, 6917); // sample values, yours will be different

void setup() {
    ...
    sensor.init();
    ...
}
```

To use alternate function timers, set the PinName directly:

```
sensor._pin = PB_7_ALT1;  // manually set a PinName to use alternate timer function
```

PWM sensor may have a slow update time (not more often than once per PWM-period, e.g. at the PWM frequency):

```
sensor.min_elapsed_time = 0.001; // 1ms minimum sample time for velocity
```
