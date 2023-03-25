
#pragma once

#include "./SimpleFOCRegisters.h"
#include "common/base_classes/FOCMotor.h"



class RegisterReceiver {
protected:
    virtual void setRegister(SimpleFOCRegister reg, void* value, FOCMotor* motor);
    virtual uint8_t readByte(void* valueToSet) = 0;
    virtual uint8_t readFloat(void* valueToSet) = 0;
    virtual uint8_t readInt(void* valueToSet) = 0;
};
