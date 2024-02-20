
# HysteresisSensor

A simple wrapper sensor which adds a configurable amount of hysteresis to any other sensor.

Set the amount of hysteresis, in rads. The hysteresis window will have a width of twice the set
amount.

## Usage

```c++
// for example, using a MA730 as the real sensor
MagneticSensorMA730 sensor = MagneticSensorMA730(PIN_nCS);
// wrapping sensor with hysteresis
HysteresisSensor hysteresisSensor = HysteresisSensor(sensor, 0.01f);

void setup() {
    ...

    // init real sensor
    sensor.init();
    hysteresisSensor.init();
    motor.linkSensor(&hysteresisSensor);

    ...

    // it may be better to run the calibration without hysteresis...
    hysteresisSensor._amount = 0.0f;
    motor.init();
    motor.initFOC();
    // switch it back on afterwards
    hysteresisSensor._amount = 0.01f;

    ...
}
```