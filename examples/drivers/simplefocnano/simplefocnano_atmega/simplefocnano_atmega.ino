#include <Arduino.h>
#include <SPI.h>
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "drivers/simplefocnano/SimpleFOCNanoDriver.h"
#include "encoders/as5048a/MagneticSensorAS5048A.h"


MagneticSensorAS5048A sensor = MagneticSensorAS5048A(PIN_nCS);
SimpleFOCNanoDriver driver = SimpleFOCNanoDriver();
BLDCMotor motor = BLDCMotor(11);    // 11 pole pairs

long loopts = 0;
int iterations = 0;
float volts = 0.0f;
 
void setup() {
  Serial.begin(115200);             // enable serial port
  delay(5000);
  SimpleFOCDebug::enable();         // enable debug messages to Serial

  sensor.init();                    // init sensor on default SPI pins

  // read voltage
  SimpleFOCDebug::print("Bus voltage: ");
  volts = driver.getBusVoltage(5.0f, 1024); // 5V reference, 10-bit ADC
  SimpleFOCDebug::println(volts);
  driver.voltage_power_supply = volts;  // set driver voltage to measured value
  driver.voltage_limit = 10.0f;         // limit voltage to 10V
  driver.pwm_frequency = 30000;         // set pwm frequency to 30kHz
  driver.init();                        // init driver

  motor.linkSensor(&sensor);        // link the motor to the sensor
  motor.linkDriver(&driver);        // link the motor to the driver

  motor.controller = MotionControlType::torque;             // torque control
  motor.torque_controller = TorqueControlType::voltage;     // use voltage torque control
  motor.voltage_limit = driver.voltage_limit / 2.0f;        // limit voltage to 1/2 of driver limit
  motor.voltage_sensor_align = 4.0f;                        // align voltage sensor to 4V

  motor.init();                     // init motor
  delay(100);                       // wait for driver to power up
  motor.initFOC();                  // init FOC and calibrate motor

  Serial.println("Motor ready.");
  loopts = millis();

  // Set the motor target to 2 volts
  motor.target = 2.0f;
}


void loop() {
  motor.move();
  motor.loopFOC();
  long now = millis();
  iterations++;
  if (now - loopts > 1000) {
    Serial.print("Iterations/s: ");
    Serial.println(iterations);
    Serial.print("Angle: ");
    Serial.println(sensor.getAngle());    
    loopts = now;
    iterations = 0;
  }
  if (now - loopts < 0)
    loopts = 0;
}
