
#include "./RegisterBasedCommander.h"

void RegisterBasedCommander::addMotor(FOCMotor* motor){
    if (numMotors < COMMS_MAX_MOTORS_COMMANDABLE){
        motors[numMotors] = motor;
        numMotors++;
    }
};
