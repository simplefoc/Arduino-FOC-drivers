
# STM32 Low Power Encoder driver

This is a driver for the encoder mode of STM32 MCU LPTIM timers.

:warning: not yet tested

The LPTIM timers are low power timers available on some STM32 MCUs. Some LPTIM timers have an encoder mode (typically LPTIM1 and LPTIM2). Using this encoder class has zero MCU overhead as the hardware handles all the encoder counting. There is also no need for interrupt handlers, interrupts are not used. However, you must be sure to call `encoder.update()` frequently (at least 2x per revolution) to correctly keep track of the full rotations.

In theory the LPTIM timers can keep counting when the MCU is in sleep mode, but in practice tracking the full rotations would be difficult to achieve.

## Compatibility

Unfortunately, Arduino framework (stm32duino) provides no support for low power timers, and there is no "PinMap" to identify which pins are LPTIM outputs.

If you want to use this driver, your MCU has to be part of the custom `PinMap_LPTIM.c` file used by this driver. Currently, we've added the following MCUs:

- STM32G431KB
- STM32G431CB
- STM32G473QE
- STM32G431VB

Adding MCUs is not hard if you look at the format [in the file](./PinMap_LPTIM.c), and pull requests for other MCUs would be gladly accepted.

## Usage

Like all our encoders, you must specify the PPR (pulses per revolution). This number should come from your encoder's datasheet. Note: internally, the encoder will use a 4x higher CPR (counts per revolution) than the PPR value you provide.

You may need to enable the LPTIM in stm32duino:

```
-DHAL_LPTIM_MODULE_ENABLED
```

Its easy to use:

```c++
STM32LowPowerEncoder encoder = STM32LowPowerEncoder(4096, PB5, PB7); // 4096 is the PPR

vpid setup() {
    ...
    encoder.init();
    motor.linkSensor(&encoder);
    ...
}
```