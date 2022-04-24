
#include "./ASCIIRegisterSender.h"

ASCIIRegisterSender::ASCIIRegisterSender(int floatPrecision) : floatPrecision(floatPrecision) {
    // nix
};

ASCIIRegisterSender::~ASCIIRegisterSender(){
 
};

void ASCIIRegisterSender::init(Print *print){
    this->_print = print;
};

uint8_t ASCIIRegisterSender::writeChar(char value){
    return _print->print(value);
};

uint8_t ASCIIRegisterSender::writeByte(uint8_t value){
    return _print->print(value);
};


uint8_t ASCIIRegisterSender::writeFloat(float value){
    return _print->print(value, floatPrecision);
};


uint8_t ASCIIRegisterSender::writeInt(uint32_t value){
    return _print->print(value);    
};