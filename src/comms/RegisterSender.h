

#ifndef __REGISTERSENDER_H__
#define __REGISTERSENDER_H__

#include "./SimpleFOCRegisters.h"
#include "common/base_classes/FOCMotor.h"

#ifndef TELEMETRY_MAX_MOTORS
#define TELEMETRY_MAX_MOTORS 4
#endif


/**
 * Register sending functionality is shared by Commander and Telemetry implementations.
 * Since the code to access all the registers is quite large, it makes sense to abstract it out,
 * and also make sure registers are addressed in the same way for all implementations.
 */
class RegisterSender {
public:
    virtual bool sendRegister(SimpleFOCRegister reg, uint8_t motorNum = 0);
    void addMotor(FOCMotor& motor);
    
    virtual uint8_t writeByte(uint8_t value) = 0;
    virtual uint8_t writeFloat(float value) = 0;
    virtual uint8_t writeInt(uint32_t value) = 0;
protected:
    FOCMotor* motors[TELEMETRY_MAX_MOTORS];
    uint8_t numMotors = 0;
};


#endif