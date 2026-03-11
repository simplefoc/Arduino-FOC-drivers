# Calibrated Sensor

Originally implemented by [@MarethyuPrefect](https://github.com/MarethyuPrefect)

A SimpleFOC Sensor wrapper that corrects for **eccentricity errors** in magnetic sensor measurements. It builds a lookup table (LUT) during calibration to linearize sensor output and improve torque control accuracy.

**More info**: See the [SimpleFOC forum thread](https://community.simplefoc.com/t/simplefoc-sensor-eccentricity-calibration/2212) for detailed discussion.

## The Problem: Sensor Eccentricity

When mounting a magnetic sensor on a motor, the magnet and sensor centers are rarely perfectly aligned. This **eccentricity error** causes:
- Non-linear sensor output across the rotor's range
- Variable measurement errors as the motor rotates
- Distortion in I_q (torque) control, reducing control performance

Example: A 0.5mm offset on a 3mm-diameter magnet can introduce ~10% error in angle measurement.

## The Solution: Calibration LUT

CalibratedSensor runs a calibration routine that:
1. Rotates the motor through several full turns
2. Records raw sensor readings at fixed intervals (creates lookup table)
3. Outputs corrective offset values (or stores them on disk)
4. Applies the LUT to sensor readings in real-time, linearizing output

Result: Non-linearity is greatly reduced, improving torque control loop stability and accuracy.


## Hardware setup

Connect your sensor as usual. Make sure the sensor is working 'normally' i.e. without calibration first. Once things are working and tuned without sensor calibration, you can add the `CalibratedSensor` to see if you get an improvement.

Note that during calibration, the motor is turned through several turns, and should be in an unloaded condition. Please ensure your hardware setup can support the motor rotating through full turns.


## Softwate setup

The `CalibratedSensor` acts as a wrapper to the actual `Sensor` class. When creating the `CalibratedSensor` object, provide the real sensor to the constructor of `CalibratedSensor`.

First, initialize the real sensor instance as normal. Then, call `calibrate()` on the `CalibratedSensor` instance. Then link the `CalibratedSensor` to the motor and call `motor.initFOC()`.

The motor will then use the calibrated sensor instance.


```c++
// magnetic sensor instance - SPI
MagneticSensorSPI sensor = MagneticSensorSPI(AS5048_SPI, PB6);
// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(11);
BLDCDriver3PWM driver = BLDCDriver3PWM(PB4,PC7,PB10,PA9);
// instantiate the calibrated sensor, providing the real sensor as a constructor argument
CalibratedSensor sensor_calibrated = CalibratedSensor(sensor);

void setup() {
  // use monitoring with serial 
  Serial.begin(115200);
  // enable debug output
  SimpleFOCDebug::enable(&Serial);

  sensor.init();
  // Link motor to sensor
  motor.linkSensor(&sensor);
  // power supply voltage
  driver.voltage_power_supply = 20;
  driver.init();
  motor.linkDriver(&driver);
  // comment out if not needed
  motor.useMonitoring(Serial);

  // initialize motor
  motor.init();

  // set voltage to run calibration
  sensor_calibrated.voltage_calibration = 6;
  // Running calibration
  sensor_calibrated.calibrate(motor); 
  // Linking sensor to motor object
  motor.linkSensor(&sensor_calibrated);

  // calibrated init FOC
  motor.initFOC();
}
```

Please see the more complete [example](https://github.com/simplefoc/Arduino-FOC-drivers/blob/master/examples/encoders/calibrated/sensor_calibration.ino) in our examples directory.

## Saving the LUT persistently in the code
After running the calibration once, you can save the generated LUT and load it on startup to avoid recalibration on each startup.

Make sure to call `calibrate()` once to generate the LUT, then use `printLUT()` to output the LUT to Serial. 

```cpp
sensor_calibrated.calibrate(motor); // run the calibration
sensor_calibrated.printLUT(motor, Serial); // print the LUT to serial monitor
```

Copy the output and paste it into your code as shown below.

```
...
SEN_CAL: Starting Sensor Calibration.
MOT: sensor_direction==CCW
MOT: PP check: OK!
MOT: Zero elec. angle: 2.83
MOT: No current sense.
MOT: Ready.
SEN_CAL: Rotating: CCW
SEN_CAL: Rotating: CW
SEN_CAL: Average Zero Electrical Angle: 2.74
SEN_CAL: Constructing LUT.
SEN_CAL: Sensor Calibration Done.

// Calibrated Sensor LUT
uint16_t calibrationLut[200] = {32701, 32694, 32689, 32684, 32678, 32671, 32665, 32661, 32656, 32649, 32644, 32641, 32638, 32634, 32629, 32624, 32619, 32612, 32609, 32607, 32604, 32600, 32596, 32594, 32594, 32591, 32588, 32587, 32588, 32592, 32593, 32594, 32594, 32594, 32597, 32602, 32605, 32609, 32614, 32620, 32627, 32632, 32636, 32642, 32649, 32658, 32666, 32673, 32681, 32687, 32692, 32702, 32711, 32719, 32725, 32732, 32740, 32748, 32755, 32760, 32766, 32773, 32781, 32787, 32792, 32796, 32797, 32800, 32804, 32807, 32809, 32809, 32810, 32810, 32812, 32810, 32808, 32808, 32808, 32808, 32807, 32804, 32804, 32803, 32802, 32803, 32805, 32805, 32804, 32803, 32803, 32805, 32804, 32802, 32803, 32805, 32820, 32824, 32826, 32823, 32817, 32810, 32807, 32807, 32807, 32805, 32803, 32803, 32806, 32809, 32808, 32807, 32806, 32804, 32802, 32799, 32796, 32791, 32791, 32791, 32789, 32787, 32786, 32786, 32785, 32783, 32781, 32783, 32784, 32786, 32788, 32790, 32790, 32791, 32791, 32795, 32800, 32803, 32806, 32810, 32815, 32820, 32824, 32827, 32832, 32838, 32845, 32850, 32856, 32860, 32863, 32868, 32874, 32879, 32883, 32886, 32890, 32894, 32899, 32902, 32903, 32906, 32911, 32912, 32911, 32909, 32905, 32903, 32902, 32899, 32895, 32891, 32886, 32883, 32879, 32872, 32866, 32860, 32856, 32850, 32843, 32835, 32827, 32818, 32810, 32804, 32798, 32790, 32781, 32774, 32768, 32761, 32753, 32745, 32739, 32735, 32729, 32722, 32716, 32708};
float zero_electric_angle = 2.74;
Direction sensor_direction = Direction::CCW;
...
```

The LUT and sensor's zero angle and direction are outputed by the calibration process to the Serial terminal. So you can copy and paste them into your code.

## Workflow: Fast Recalibration Using Saved LUT

If you calibrate once during setup and save the LUT to EEPROM or hardcode it, you can skip re-calibration on every startup:

1. **First run**: Call `calibrate(motor)` → Serial outputs LUT
2. **Copy LUT**: Paste the generated values into your code
3. **Subsequent runs**: Pass LUT to constructor and DO NOT call `calibrate()` → instantaneous, no rotation needed

Your code will look something like this:

```c++

// number of LUT entries
const N_LUT = 200;
// Lookup table that has been output from the calibration process
// The LUT is now stored as uint16_t for 50% memory savings (2 bytes vs 4 bytes per entry)
// Calibrated Sensor LUT
uint16_t calibrationLut[200] = {32701, 32694, 32689, 32684, 32678, 32671, 32665, 32661, 32656, 32649, 32644, 32641, 32638, 32634, 32629, 32624, 32619, 32612, 32609, 32607, 32604, 32600, 32596, 32594, 32594, 32591, 32588, 32587, 32588, 32592, 32593, 32594, 32594, 32594, 32597, 32602, 32605, 32609, 32614, 32620, 32627, 32632, 32636, 32642, 32649, 32658, 32666, 32673, 32681, 32687, 32692, 32702, 32711, 32719, 32725, 32732, 32740, 32748, 32755, 32760, 32766, 32773, 32781, 32787, 32792, 32796, 32797, 32800, 32804, 32807, 32809, 32809, 32810, 32810, 32812, 32810, 32808, 32808, 32808, 32808, 32807, 32804, 32804, 32803, 32802, 32803, 32805, 32805, 32804, 32803, 32803, 32805, 32804, 32802, 32803, 32805, 32820, 32824, 32826, 32823, 32817, 32810, 32807, 32807, 32807, 32805, 32803, 32803, 32806, 32809, 32808, 32807, 32806, 32804, 32802, 32799, 32796, 32791, 32791, 32791, 32789, 32787, 32786, 32786, 32785, 32783, 32781, 32783, 32784, 32786, 32788, 32790, 32790, 32791, 32791, 32795, 32800, 32803, 32806, 32810, 32815, 32820, 32824, 32827, 32832, 32838, 32845, 32850, 32856, 32860, 32863, 32868, 32874, 32879, 32883, 32886, 32890, 32894, 32899, 32902, 32903, 32906, 32911, 32912, 32911, 32909, 32905, 32903, 32902, 32899, 32895, 32891, 32886, 32883, 32879, 32872, 32866, 32860, 32856, 32850, 32843, 32835, 32827, 32818, 32810, 32804, 32798, 32790, 32781, 32774, 32768, 32761, 32753, 32745, 32739, 32735, 32729, 32722, 32716, 32708};
float zero_electric_angle = 2.74;
Direction sensor_direction = Direction::CCW;

// provide the sensor class and the number of points in the LUT
CalibratedSensor sensor_calibrated = CalibratedSensor(sensor, N_LUT, calibrationLut);

... 

void setup() {
  ...
  // No need to call calibrate() any more as the LUT is provided
  // Only link the calibrated sensor to the motor
  motor.linkSensor(&sensor_calibrated);
  // provide the saved zero angle and direction
  motor.zero_electric_angle = zero_electric_angle;
  motor.sensor_direction = sensor_direction;

  ... 
  motor.initFOC();
  ....
}


```

## Changelog

#### **Dec 2024** 
- A rewrite to reduce memory usage passed to `uint16_t` LUT type. 
    - Previous `float` LUT used 4 bytes per entry, `uint16_t` uses 2 bytes - a 50% reduction.
    - The LUT values are scaled internally to maintain precision.
    - Precision is 2PI/65536 radians (~0.005 degrees), which is sufficient for most applications.  If more precision is needed, consider increasing the LUT_SCALE constant in `CalibratedSensor.h`.
- Fixed bug due to hardcoded number of sampled positions.
    - Previously, the LUT size was variable but the number of sampled positions was hardcoded to `pole_pairs * 5`. 
    - Now the number of samples is derived from the LUT size provided to the constructor. 


## Future work

- Use a more efficient LUT interpolation method - maybe a polynomial interpolation
- Support for saving/loading LUT to/from persistent storage (EEPROM, Flash)
