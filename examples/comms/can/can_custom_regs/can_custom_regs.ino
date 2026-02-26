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

    // add custom register to control built-in LED
    pinMode(LED_BUILTIN, OUTPUT);
    commander.addCustomRegister(
        0xF0, // register id (should be > 0xE0) 
        1, // size in bytes
        [](RegisterIO& comms, FOCMotor* motor) -> bool {
            // Read handler for custom register 0xF0
            uint8_t customValue = digitalRead(LED_BUILTIN);
            comms << customValue;
            return true;
        },
        [](RegisterIO& comms, FOCMotor* motor) -> bool {
            // Write handler for custom register 0xF0
            uint8_t receivedValue;
            comms >> receivedValue;
            digitalWrite(LED_BUILTIN, receivedValue ? HIGH : LOW);
            return true;
        }
    );

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