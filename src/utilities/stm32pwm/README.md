
# STM32 Utilities - PWM Input

Using the `STM32PWMInput` class you can precisely read a PWM input signal on STM32 MCUs by using their timer's PWM-Input mode. This happens entirely in the timer hardware with zero MCU overhead, and is very precise.

## Setup

Connect the PWM input to either channel 1 or channel 2 of a general purpose or advanced control timer supporting PWM-Input mode.

This should include the following timers:

- Advanced control timers: TIM1, TIM8
- General purpose timers (32 bit): TIM2, TIM5
- General purpose timers (16 bit): TIM3, TIM4, TIM9, TIM12

The best to use are the 32 bit general purpose timers, although TIM1 may also be interesting because on some MCUs it can be clocked at a higher rate than the other timers.

## Usage

```
  STM32PWMInput pwmInput = STM32PWMInput(PA15);
  pwmInput.init();

  uint32_t periodTicks = pwmInput.getPeriodTicks();
  uint32_t dutyTicks = pwmInput.getDutyCycleTicks();
  float dutyPercent = pwmInput.getDutyCyclePercent();
```

## Input signal

The input signal should be a PWM signal (square wave) with a duty cycle that is >0% and <100%. The 

The behaviour if the input is not a square wave is not defined, although the MCU will continue to sample the input, and resumes correct measurement when the square wave input is restored.

## Performance

The range of PWM frequencies that can be measured and the resolution with which they can be sampled depends on the following:

- timer clock speed - normally this is your MCU speed, but it can be lower, or in some cases even higher. 
- timer prescaler - can divide the timer clock.

The sample resolution for a PWM input signal of frequency _Fp_ is given by

_R = Fc / Fp_

Where _R_ is the resolution in ticks, and _Fc_ is the timer tick frequency (timer clock with prescaler).

This sample resolution is equal to the length of the PWM period in ticks. The timer needs to sample both the duty cycle and the full period, so the timer can't sample signals whose period would cause it to overflow. 

On a 16 bit timer this means the period (and sample resolution) must be less than 65536 ticks, while on a 32 bit timer it must be less than 4294967296 ticks. This places a lower limit on the PWM frequencies that can be sampled.

On the other side, the minimum duty cycle that can be reliably captured is limited by the duration of one timer tick, e.g. the minimum on-time that can be measured reliably is equal to two timer ticks in duration.

So as the frequency increases, the resolution decreases, and the minimum duty cycle increases.

For example, on a 64MHz STM32 MCU, using TIM3 (16 bit) you could capture a PWM signal at 10kHz with a resolution of 6400 ticks. This fits in the 16 bit timer, so no problem. Assuming the signal is perfectly stable (it usually isn't) that would be 12 bits of accuracy on your input. 

If you lowered the PWM input frequency to 1kHz, you'd get a 64000 range - just fitting in the timer. But now the accuracy is greatly increased - almost 16 bits!

If you increased the PWM frequency to 10Mhz, the resolution would be just 6 ticks, and the resulting accuracy very low - just 2 bits, with a minimum duty cycle of 33% and a maximum of 66%.

Of course you could vary the 10MHz signal faster than the 1kHz one - so choosing the PWM input frequency is a tradeoff between accuracy and control bandwidth...

Note: clock configuration is out of scope for this class. Set your clocks up in advance.



## Roadmap

- Support setting of the filtering function, this could help a lot against noise on the input
- Support setting of the timer prescaler (not the clock prescaler!)
- Support setting of the downsample function, this could help increase accuracy
- Support choosing of the alternate timers on pins with more than one
