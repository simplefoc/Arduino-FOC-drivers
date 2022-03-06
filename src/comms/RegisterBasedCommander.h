
#ifndef __REGISTER_BASED_COMMS_DRIVER_H__
#define __REGISTER_BASED_COMMS_DRIVER_H__



#include "./SimpleFOCRegisters.h"
#include "common/base_classes/FOCMotor.h"


#ifndef COMMS_MAX_MOTORS_COMMANDABLE
#define COMMS_MAX_MOTORS_COMMANDABLE 4
#endif
#ifndef COMMS_MAX_REPORT_REGISTERS
#define COMMS_MAX_REPORT_REGISTERS 7
#endif



class RegisterBasedCommander {
public:
    virtual void addMotor(FOCMotor* motor) = 0;
    virtual void runCommander() = 0;
protected:
    virtual bool sendRegister(SimpleFOCRegister reg, uint8_t motorNum = 0) = 0;
    virtual void receiveRegister(SimpleFOCRegister reg, uint8_t motorNum = 0) = 0;
    virtual void setRegister(SimpleFOCRegister reg, void* value, uint8_t motorNum = 0) = 0;
    uint8_t numMotors = 0;
    uint8_t curMotor = 0;
    SimpleFOCRegister curRegister = REG_STATUS;
    bool commanderror = false;
    bool lastcommanderror = false;
    uint8_t lastcommandregister = REG_STATUS;
    FOCMotor* motors[COMMS_MAX_MOTORS_COMMANDABLE];
    uint8_t numReportRegisters = 0;
    uint8_t reportMotors[COMMS_MAX_REPORT_REGISTERS];
    SimpleFOCRegister reportRegisters[COMMS_MAX_REPORT_REGISTERS];        
};



#endif
