/**
 * Torque control example using voltage control loop.
 * 
 * Most of the low-end BLDC driver boards doesn't have current measurement therefore SimpleFOC offers 
 * you a way to control motor torque by setting the voltage to the motor instead hte current. 
 * 
 * This makes the BLDC motor effectively a DC motor, and you can use it in a same way.
 */
#include <SimpleFOC.h>
#include <SimpleFOCDrivers.h>
#include "encoders/calibrated/CalibratedSensor.h"

// magnetic sensor instance - SPI
MagneticSensorSPI sensor = MagneticSensorSPI(AS5048_SPI, PB6);
// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(11);
BLDCDriver3PWM driver = BLDCDriver3PWM(PB4,PC7,PB10,PA9);
// instantiate the calibrated sensor object
CalibratedSensor sensor_calibrated = CalibratedSensor(sensor);

// voltage set point variable
float target_voltage = 2;

// instantiate the commander
Commander command = Commander(Serial);

void doTarget(char* cmd) { command.scalar(&target_voltage, cmd); }

void setup() {

  SPI.setMISO(PB14);
  SPI.setMOSI(PB15);
  SPI.setSCLK(PB13);

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

  // add target command T
  command.add('T', doTarget, "target voltage");
  
  Serial.println(F("Motor ready."));

  Serial.println(F("Set the target voltage using serial terminal:"));
  _delay(1000);
}

void loop() {

  motor.loopFOC();
  motor.move(target_voltage);
  command.run();
  motor.monitor();
 
}