#include <Arduino.h>
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "drivers/dc/DCDriverSpeedDir.h"
#include "motors/dc/DCMotor.h"
#include "encoders/as5048a/MagneticSensorAS5048A.h"

// closed loop control needs a sensor - make sure you use your pin numbers
MagneticSensorAS5048A sensor = MagneticSensorAS5048A(14);
// we use a simple driver module with speed and direction inputs. It also has an enable pin - make sure you use your pin numbers
DCDriverSpeedDir driver = DCDriverSpeedDir(15, 16, 17);
// Motor class
DCMotor motor = DCMotor();

// Commander for serial port control
Commander commander = Commander(Serial, '\n', false);
void onMotor(char* cmd){commander.motor(&motor, cmd);}



void setup() {
  Serial.begin(115200);
  SimpleFOCDebug::enable(&Serial);

  // set some paramters
  driver.voltage_power_supply = 12.0f;
  driver.voltage_limit = 8.0f;
  motor.controller = MotionControlType::torque;
  motor.torque_controller = TorqueControlType::voltage;

  // intialize everything
  sensor.init();
  driver.init();
  motor.linkSensor(&sensor);
  motor.linkDriver(&driver);
  motor.init();

  commander.add('M', onMotor, "DC Motor");

  SimpleFOCDebug::println("Motor initialized.");
}


void loop() {
  motor.move();
  commander.run();

  // 4kHz is enough for good control. If you go too fast you will oversample the sensor.
  // on a slow MCU reduce this value or comment out this line.
  delayMicroseconds(250); 
}