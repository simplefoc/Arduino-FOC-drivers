# Calibrated Sensor

by [@MarethyuPrefect](https://github.com/MarethyuPrefect)

A SimpleFOC Sensor wrapper implementation which adds sensor eccentricity calibration.

Please also see our [forum thread](https://community.simplefoc.com/t/simplefoc-sensor-eccentricity-calibration/2212) on this topic.


When you mount your (magnetic) sensor on your frame or motor, there will always be a slight misalignment between magnet and sensor (measurement system). This misalignment between center of rotation and the center of the sensor is called the eccentricity error.

As a result your measurement system output is non-linear with respect to the rotor of the motor. This will cause an error with respect to the ideal torque you attempt to create with the I_q vector as function of the position. You could interpret this as a disturbance on your control loop which you want to minimize for optimal performance. 

This calibration compensates the sensor reading in a feed forward fashion such that your performance improves.


## Hardware setup

Connect your sensor as usual. Make sure the sensor is working 'normally' i.e. without calibration first. Once things are working and tuned without sensor calibration, you can add the CalibratedSensor to see if you get an improvement.

Note that during calibration, the motor is turned through several turns, and should be in an unloaded condition. Please ensure your hardware setup can support the motor rotating through full turns.


## Softwate setup

The CalibratedSensor acts as a wrapper to the actual sensor class. When creating the CalibratedSensor object, provide the real
sensor to the constructor of CalibratedSensor.

First, initialize the real sensor instance as normal. Then, call calibrate() on the CalibratedSensor instance. Then link the 
CalibratedSensor to the motor and call motor.initFOC().

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
  sensor.init();
  // Link motor to sensor
  motor.linkSensor(&sensor);
  // power supply voltage
  driver.voltage_power_supply = 20;
  driver.init();
  motor.linkDriver(&driver);
  // aligning voltage 
  motor.voltage_sensor_align = 8;
  motor.voltage_limit = 20;
  // set motion control loop to be used
  motor.controller = MotionControlType::torque;

  // use monitoring with serial 
  Serial.begin(115200);
  // comment out if not needed
  motor.useMonitoring(Serial);
  motor.monitor_variables =  _MON_VEL; 
  motor.monitor_downsample = 10; // default 10

  // initialize motor
  motor.init();

  // set voltage to run calibration
  sensor_calibrated.voltage_calibration = 6;
  // Running calibration
  sensor_calibrated.calibrate(motor); 

  //Serial.println("Calibrating Sensor Done.");
  // Linking sensor to motor object
  motor.linkSensor(&sensor_calibrated);

  // calibrated init FOC
  motor.initFOC();
}
```

Please see the more complete [example](https://github.com/simplefoc/Arduino-FOC-drivers/blob/master/examples/encoders/calibrated/sensor_calibration.ino) in our examples directory.

## EDIT March 2025

The code has been rewritten to reduce its memory footprint and allow more flexible Lookup table (LUT) sizing. 
Additionally, the calibrated sensor class now supports providing the saved LUT as a paramer for calibration. This allows you to save the LUT and load it on startup to avoid recalibration on each startup.

Once you do the calibration once, it will output something like this:

```
...

Starting Sensor Calibration.
MOT: Align sensor.
MOT: sensor_direction==CCW
MOT: PP check: OK!
MOT: Zero elec. angle: 3.17
MOT: No current sense.
MOT: Ready.Rotating: CCW
Rotating: CW
Average Zero Electrical Angle: 4.01
Constructing LUT.

uint16_t calibrationLut[50] = {32803, 32828, 32843, 32854, 32859, 32846, 32830, 32810, 32781, 32753, 32725, 32689, 32658, 32631, 32612, 32586, 32576, 32568, 32571, 32588, 32609, 32638, 32678, 32726, 32775, 32822, 32870, 32906, 32939, 32956, 32959, 32944, 32941, 32929, 32909, 32887, 32858, 32831, 32800, 32771, 32743, 32722, 32706, 32694, 32693, 32693, 32705, 32728, 32749, 32777};
float zero_electric_angle = 4.007072;
Direction sensor_direction = Direction::CCW;
Sensor Calibration Done
...
```

The LUT and sensor's zero angle and direction are outputed by the calibration process to the Serial terminal. So you can copy and paste them into your code.

Your code will look something like this:

```c++

// number of LUT entries
const N_LUT = 50;
// Lookup table that has been output from the calibration process
// The LUT is now stored as uint16_t for 50% memory savings (2 bytes vs 4 bytes per entry)
uint16_t calibrationLut[50] = {32803, 32828, 32843, 32854, 32859, 32846, 32830, 32810, 32781, 32753, 32725, 32689, 32658, 32631, 32612, 32586, 32576, 32568, 32571, 32588, 32609, 32638, 32678, 32726, 32775, 32822, 32870, 32906, 32939, 32956, 32959, 32944, 32941, 32929, 32909, 32887, 32858, 32831, 32800, 32771, 32743, 32722, 32706, 32694, 32693, 32693, 32705, 32728, 32749, 32777};
float zero_electric_angle = 4.007072;
Direction sensor_direction = Direction::CCW;

// provide the sensor class and the number of points in the LUT
CalibratedSensor sensor_calibrated = CalibratedSensor(sensor, N_LUT);

... 

void setup() {
  ...
  // as LUT is provided to this function
  sensor_calibrated.calibrate(motor, calibrationLut, zero_eletrical_angle, sensor_direction);
  ...

  motor.linkSensor(&sensor_calibrated);

  ... 
  motor.initFOC();
  ....
}


```

## Implementation Details

### Memory Optimization

The LUT is now stored using `uint16_t` instead of `float`, providing a 50% reduction in memory usage:
- **Old**: 4 bytes per entry (float)
- **New**: 2 bytes per entry (uint16_t)
- **Example**: For a 200-point LUT: 800 bytes → 400 bytes

The quantization maps the offset range [-π, π] radians to [0, 65535], providing a resolution of approximately 0.0001 radians (0.0057°), which is more than sufficient for motor control applications.

## Future work

- Use a more efficient LUT interpolation method - maybe a polynomial interpolation
- Support for saving/loading LUT to/from persistent storage (EEPROM, Flash)
