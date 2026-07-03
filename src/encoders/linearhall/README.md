# LinearHall

by [@nanoparticle](https://github.com/nanoparticle), [@XieMaster](https://github.com/XieMaster) and [@dekutree64](https://github.com/dekutree64)

This sensor class is for two linear hall effect sensors such as 49E to sense the rotor magnets of an outrunner or a diametric magnet mounted on the shaft (set pp to 1). The sensors can be spaced 60, 90 or 120 electrical degrees apart. 90 degrees will give a slightly more accurate reading (if one is centered on a rotor magnet, the other is half way between magnets), but sometimes it's convenient to position them 120 degrees like digital hall sensors, or 60 degrees which is equivalent to 120 with one sensor flipped to opposite polarity.

Optimal distance of sensors to magnets is typically between 1mm and 3mm. Too close and the reading will saturate and become trapezoidal, too far and resolution diminishes. It can still work fairly well with as little as ±50 ADC units of resolution.

The Arduino function analogRead has poor performance on some platforms such as STM32 and RP2350. The ReadLinearHalls function is declared "weaK" so you can override it with custom ADC code without having to modify the library code.

Please also see our [forum thread](https://community.simplefoc.com/t/40-cent-magnetic-angle-sensing-technique/1959) on this topic, and this PDF for more detail on sensor placement https://gist.github.com/nanoparticle/00030ea27c59649edbed84f0a957ebe1


## Example usage
```c++
#include <SimpleFOC.h>
#include <SimpleFOCDrivers.h>
#include <encoders/linearhall/LinearHall.h>

BLDCMotor motor = BLDCMotor(11);
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 5, 6, 8);
LinearHall sensor = LinearHall(A0, A1, 11, LinearHall::SensorSpacing::_90);

void setup() {
  Serial.begin(115200);
  motor.useMonitoring(Serial); // LinearHall uses this to print out the calibration results

  driver.voltage_power_supply = 12;
  driver.init();
  motor.linkDriver(&driver);
  motor.init();
  // initialize sensor hardware. This moves the motor to find the min/max sensor readings and 
  // averages them to get the center values. The motor can't move until motor.init is called, and 
  // motor.initFOC can't do its calibration until the sensor is intialized, so this must be done inbetween.
  // You can then take the values printed to the serial monitor and pass them to sensor.init to 
  // avoid having to move the motor every time. In that case it doesn't matter whether sensor.init 
  // is called before or after motor.init.
  sensor.init(&motor);
  motor.linkSensor(&sensor);
  motor.initFOC();
}
```

## Future work

- High-performance ADC configurations on platforms with poor analogRead performance.
- Support for redundancy using 3 sensors spaced 120 degrees. Probably best to make a subclass.