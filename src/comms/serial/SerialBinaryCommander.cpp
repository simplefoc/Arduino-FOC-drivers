
#include "SerialBinaryCommander.h"



SerialBinaryCommander::SerialBinaryCommander(bool echo) : BinaryTelemetry(this), echo(echo) {

};



SerialBinaryCommander::~SerialBinaryCommander(){

};



void SerialBinaryCommander::init(Stream &serial) {
    _serial = &serial;
    this->Telemetry::init();
};



void SerialBinaryCommander::runCommander() {
    if (_serial->available()>2) { // TODO make this work with partial packets...
        uint8_t command = _serial->read();
        uint8_t registerNum = _serial->read();
        uint8_t motorNum = _serial->read();
        if (command==SERIALBINARY_COMMAND_READ){
            startFrame(registerNum, SERIALBINARY_FRAMETYPE_REGISTER);
            sendRegister(static_cast<SimpleFOCRegister>(registerNum), motorNum);
            endFrame();
        }
        else if (command==SERIALBINARY_COMMAND_WRITE) {
            receiveRegister(static_cast<SimpleFOCRegister>(registerNum), motorNum);
            if (echo) {
                startFrame(registerNum, SERIALBINARY_FRAMETYPE_REGISTER);
                sendRegister(static_cast<SimpleFOCRegister>(registerNum), motorNum);
                endFrame();
            }
        }
    }
};



uint8_t SerialBinaryCommander::readBytes(void* valueToSet, uint8_t numBytes){
    if (_serial->available()<numBytes)
        return 0;
    uint8_t* value = (uint8_t*)valueToSet;
    for (uint8_t i=0; i<numBytes; i++) {
        value[i] = _serial->read();
    }
    return numBytes;
};




uint8_t SerialBinaryCommander::writeBytes(void* valueToSend, uint8_t numBytes){
    uint8_t* value = (uint8_t*)valueToSend;
    for (uint8_t i=0; i<numBytes; i++) {
        _serial->write(value[i]);
    }
    return numBytes;
};






void SerialBinaryCommander::startFrame(uint8_t frameSize, uint8_t type){
    _serial->write(0xA5);
    _serial->write((uint8_t)((type<<6)|frameSize));
};





void SerialBinaryCommander::endFrame(){
    _serial->write(0x5A);
};



