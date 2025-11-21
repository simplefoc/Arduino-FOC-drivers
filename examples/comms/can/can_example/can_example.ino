#include "Arduino.h"   
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "SimpleCANio.h"
#include "comms/can/CANCommander.h"

// can pins
#define CAN_RX NC // define the RX pin
#define CAN_TX NC // define the TX pin
#define CAN_SHDN NC // define the Shhutdown pin (inverse of enable) if needed
#define CAN_ENABLE NC // define the ENABLE pin if needed
#define CAN_ID 1

// 3pwm pins
#define PHA PA6
#define PHB PA7
#define PHC PA8

BLDCMotor motor = BLDCMotor(7); // 7 pole pairs
BLDCDriver3PWM driver = BLDCDriver3PWM(PHA, PHB, PHC);

//CANio can(PIN_CAN0_RX, PIN_CAN0_TX); // Create CAN object
CANio can(CAN_RX, CAN_TX, NC, CAN_ENABLE); // Create CAN object

CANCommander commander(can, CAN_ID);
void setup()
{   
    Serial.begin(115200);
    SimpleFOCDebug::enable(&Serial);

    commander.init();
    commander.addMotor(&motor);
    delay(5000);

    motor.linkDriver(&driver);
    driver.voltage_power_supply = 12;
    driver.init();

    motor.init();
    motor.initFOC();

    Serial.println("Setup complete!");
    delay(10);
}

void loop()
{
    motor.loopFOC();
    motor.move();
  
    commander.run();
}