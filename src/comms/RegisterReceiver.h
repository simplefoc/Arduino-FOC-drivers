
#pragma once

#include "./SimpleFOCRegisters.h"
#include "common/base_classes/FOCMotor.h"



class RegisterReceiver {
protected:
    virtual void setRegister(SimpleFOCRegister reg, FOCMotor* motor);
    virtual uint8_t readByte(uint8_t* valueToSet) = 0;
    virtual uint8_t readFloat(float* valueToSet) = 0;
    virtual uint8_t readInt(uint32_t* valueToSet) = 0;
};
