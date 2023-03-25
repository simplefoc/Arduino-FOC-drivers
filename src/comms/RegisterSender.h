
#pragma once

#include "./SimpleFOCRegisters.h"
#include "common/base_classes/FOCMotor.h"

/**
 * Register sending functionality is shared by Commander and Telemetry implementations.
 * Since the code to access all the registers is quite large, it makes sense to abstract it out,
 * and also make sure registers are addressed in the same way for all implementations.
 */
class RegisterSender {
protected:
    virtual bool sendRegister(SimpleFOCRegister reg, FOCMotor* motor);
    virtual uint8_t writeByte(uint8_t value) = 0;
    virtual uint8_t writeFloat(float value) = 0;
    virtual uint8_t writeInt(uint32_t value) = 0;
};
