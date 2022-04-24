
#include "./BinaryCommander.h"



BinaryCommander::BinaryCommander() : BinaryRegisterSender(), RegisterBasedCommander() {
    // nix
};



uint8_t BinaryCommander::readByte(void* valueToSet){
    return readBytes(valueToSet, 1);
};



uint8_t BinaryCommander::readFloat(void* valueToSet){
    return readBytes(valueToSet, 4);
};



uint8_t BinaryCommander::readInt(void* valueToSet){
    return readBytes(valueToSet, 4);
};


