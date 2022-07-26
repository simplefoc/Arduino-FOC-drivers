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


## Roadmap

Possible future improvements we've thought about:

- Improve memory usage and performance
- Make calibration able to be saved/restored

