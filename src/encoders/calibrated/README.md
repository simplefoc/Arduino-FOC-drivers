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

## EDIT December 2025

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

float calibrationLut[50] = {0.003486, 0.005795, 0.007298, 0.008303, 0.008771, 0.007551, 0.005986, 0.004115, 0.001361, -0.001392, -0.004069, -0.007474, -0.010420, -0.013135, -0.014891, -0.017415, -0.018328, -0.019125, -0.018849, -0.017193, -0.015152, -0.012422, -0.008579, -0.003970, 0.000678, 0.005211, 0.009821, 0.013280, 0.016470, 0.018127, 0.018376, 0.016969, 0.016716, 0.015466, 0.013602, 0.011431, 0.008646, 0.006092, 0.003116, 0.000409, -0.002342, -0.004367, -0.005932, -0.006998, -0.007182, -0.007175, -0.006017, -0.003746, -0.001783, 0.000948};
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
// Lookup table that has been ouptut from the calibration process
float calibrationLut[50] = {0.003486, 0.005795, 0.007298, 0.008303, 0.008771, 0.007551, 0.005986, 0.004115, 0.001361, -0.001392, -0.004069, -0.007474, -0.010420, -0.013135, -0.014891, -0.017415, -0.018328, -0.019125, -0.018849, -0.017193, -0.015152, -0.012422, -0.008579, -0.003970, 0.000678, 0.005211, 0.009821, 0.013280, 0.016470, 0.018127, 0.018376, 0.016969, 0.016716, 0.015466, 0.013602, 0.011431, 0.008646, 0.006092, 0.003116, 0.000409, -0.002342, -0.004367, -0.005932, -0.006998, -0.007182, -0.007175, -0.006017, -0.003746, -0.001783, 0.000948};
float zero_electric_angle = 4.007072;
Direction sensor_direction = Direction::CCW;

// provide the sensor class and the number of points in the LUT
CalibratedSensor sensor_calibrated = CalibratedSensor(sensor, N_LUT, calibrationLut);

... 

void setup() {
  ...
  // NOTE: When providing a pre-defined LUT, the calibration step is skipped!
  // You can remove it from your code if you want.
  sensor_calibrated.calibrate(motor);
  ...

  motor.linkSensor(&sensor_calibrated);

  ... 
  motor.initFOC();
  ....
}


```

## Future work

- Reduce the LUT size by using a more efficient LUT type - maybe pass to uint16_t
- Use a more efficient LUT interpolation method - maybe a polynomial interpolation