

#include "./BinaryRegisterSender.h"


uint8_t BinaryRegisterSender::writeByte(uint8_t value){
    return writeBytes(&value, 1);
};



uint8_t BinaryRegisterSender::writeFloat(float value){
    return writeBytes(&value, 4);
};



uint8_t BinaryRegisterSender::writeInt(uint32_t value){
    return writeBytes(&value, 4);
};


