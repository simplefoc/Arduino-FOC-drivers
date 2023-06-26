# Smoothing Sensor

by [@dekutree64](https://github.com/dekutree64)

A SimpleFOC Sensor wrapper implementation which adds angle extrapolation

Please also see our [forum thread](https://community.simplefoc.com/t/smoothingsensor-experimental-sensor-angle-extrapoltion/3105) on this topic.


SmoothingSensor is a wrapper class which is inserted inbetween a sensor and motor to provide better quality angle readings from sensors that are low resolution or are slow to communicate. It provides no improvement for sensors that are high resolution and quick to update. It uses the timestamp of the last angle reading from the sensor and the low-pass filtered velocity from the motor to predict what the angle should be at the current time.


## Hardware setup

Connect your sensor as usual. Make sure the sensor is working 'normally' i.e. without smoothing first. Once things are working and tuned without smoothing, you can add the SmoothingSensor to see if you get an improvement.


## Softwate setup

The SmoothingSensor acts as a wrapper to the actual sensor class. When creating the SmoothingSensor object, provide the real sensor to the constructor of SmoothingSensor. Initialize the real sensor instance as normal. Then link the SmoothingSensor to the motor and call motor.initFOC().


```c++
// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(11);
BLDCDriver3PWM driver = BLDCDriver3PWM(PB4,PC7,PB10,PA9);
// real sensor instance
MagneticSensorPWM sensor = MagneticSensorPWM(2, 4, 904);
// instantiate the smoothing sensor, providing the real sensor as a constructor argument
SmoothingSensor smooth = SmoothingSensor(sensor, motor);

void doPWM(){sensor.handlePWM();}

void setup() {
  sensor.init();
  sensor.enableInterrupt(doPWM);
  // Link motor to sensor
  motor.linkSensor(&smooth);
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
  motor.initFOC();
}
```


## Roadmap

Possible future improvements we've thought about:

- Add extrapolation of acceleration as well
- Switch to open loop control at very low speed with hall sensors, which otherwise move in steps even with smoothing.

