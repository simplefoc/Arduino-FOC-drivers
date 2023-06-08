
# STM32SpeedDirInput

Control SimpleFOC using PWM speed and direction inputs.

Based on STM32 timer PWM-Input capabilities, which means this can only be used on STM32 MCUs. It can cover a wide range of PWM frequencies, and runs without MCU overhead in the timer hardware.



## Setup

The PWM speed input should be connected to either channel 1 or channel 2 of a general purpose or advanced control timer on your STM32 MCU. Suitable timers are:
- Advanced control timers: TIM1, TIM8
- General purpose timers (32 bit): TIM2, TIM5
- General purpose timers (16 bit): TIM3, TIM4, TIM9, TIM12
If in doubt, check in STM32CubeIDE and see if the PWM-Input mode can be enabled (under "Combined Channels") for the timer.

The optional direction input can be connected to any GPIO pin. By default a high level direction input is associated with a positive velocity value, while a low level direction input results in a negative velocity value. To reverse this, set the option `dir_positive_high = false`

The direction input is optional - if not provided, you can control the direction from software using the `direction` field.

The velocity values returned are in the range `min_speed` to `max_speed`, while the input PWM duty cycle should lie within the range `min_pwm` to `max_pwm`. Actual input values smaller than `min_pwm` will be treated as `min_pwm`, values larger than `max_pwm` will be treated as `max_pwm`. The behaviour for 100% or 0% duty cycles is undefined, and they should be avoided.

## Usage

Use it like this:

```c++
#include "comms/stm32speeddir/STM32SpeedDirInput.h"

// some example pins - the speed pin has to be on channel 1 or 2 of a timer
#define PIN_SPEED PC6
#define PIN_DIRECTION PB8

STM32SpeedDirInput speed_dir = STM32SpeedDirInput(PIN_SPEED, PIN_DIRECTION);

float target = 0.0f;

void setup(){
    ...

    speed_dir.min_speed = 10.0f;    // 10rad/s min speed
    speed_dir.max_speed = 100.0f;   // 100rad/s max speed
    speed_dir.min_pwm = 5.0f;       // 5% min duty cycle
    speed_dir.max_pwm = 95.0f;      // 95% max duty cycle
    speed_dir.init();

    ...
}


void loop(){
    target = speed_dir.getTargetVelocity();
    motor.move(target);
    motor.loopFOC();
}

```