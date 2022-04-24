
#ifndef __REGISTER_BASED_COMMS_DRIVER_H__
#define __REGISTER_BASED_COMMS_DRIVER_H__



#include "./SimpleFOCRegisters.h"
#include "./RegisterSender.h"



#ifndef COMMS_MAX_REPORT_REGISTERS
#define COMMS_MAX_REPORT_REGISTERS 7
#endif



class RegisterBasedCommander {
public:
    virtual void runCommander() = 0;
protected:
    virtual void receiveRegister(SimpleFOCRegister reg, uint8_t motorNum = 0);
    virtual void setRegister(SimpleFOCRegister reg, void* value, uint8_t motorNum = 0);
    virtual uint8_t readByte(void* valueToSet) = 0;
    virtual uint8_t readFloat(void* valueToSet) = 0;
    virtual uint8_t readInt(void* valueToSet) = 0;

    uint8_t curMotor = 0;
    SimpleFOCRegister curRegister = REG_STATUS;
    bool commanderror = false;
    bool lastcommanderror = false;
    uint8_t lastcommandregister = REG_STATUS;

    uint8_t numReportRegisters = 0;
    uint8_t reportMotors[COMMS_MAX_REPORT_REGISTERS];
    SimpleFOCRegister reportRegisters[COMMS_MAX_REPORT_REGISTERS];        
};



#endif
