
#include "./BinaryCommander.h"




uint8_t BinaryCommander::writeByte(uint8_t value){
    return writeBytes(&value, 1);
};



uint8_t BinaryCommander::writeFloat(float value){
    return writeBytes(&value, 4);
};



uint8_t BinaryCommander::writeInt(uint32_t value){
    return writeBytes(&value, 4);
};



uint8_t BinaryCommander::readByte(void* valueToSet){
    return 0;
};



uint8_t BinaryCommander::readFloat(void* valueToSet){
    return 0;
};



uint8_t BinaryCommander::readInt(void* valueToSet){
    return 0;
};


