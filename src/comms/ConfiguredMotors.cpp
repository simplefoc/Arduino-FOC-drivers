
#include "./ConfiguredMotors.h"


void ConfiguredMotors::addMotor(FOCMotor& motor) {
    if (numMotors < TELEMETRY_MAX_MOTORS) {
        motors[numMotors] = &motor;
        numMotors++;
    }
};
