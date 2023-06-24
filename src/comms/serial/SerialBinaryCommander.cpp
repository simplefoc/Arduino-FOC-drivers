
#include "SerialBinaryCommander.h"



SerialBinaryCommander::SerialBinaryCommander(bool echo) : Telemetry(), echo(echo) {

};



SerialBinaryCommander::~SerialBinaryCommander(){

};



void SerialBinaryCommander::init(Stream &serial) {
    _serial = &serial;
    this->Telemetry::init();
};



void SerialBinaryCommander::run() {
    if (_serial->available()>2) { // TODO make this work with partial packets...
        uint8_t command = _serial->read();
        uint8_t registerNum = _serial->read();
        uint8_t motorNum = _serial->read();
        if (command==SERIALBINARY_COMMAND_READ){
            startFrame(registerNum, SERIALBINARY_FRAMETYPE_REGISTER); // TODO call is incorrect
            sendRegister(static_cast<SimpleFOCRegister>(registerNum), motors[motorNum]);
            endFrame();
        }
        else if (command==SERIALBINARY_COMMAND_WRITE) {
            setRegister(static_cast<SimpleFOCRegister>(registerNum), motors[motorNum]);
            if (echo) {
                startFrame(registerNum, SERIALBINARY_FRAMETYPE_REGISTER); // TODO call is incorrect
                sendRegister(static_cast<SimpleFOCRegister>(registerNum), motors[motorNum]);
                endFrame();
            }
        }
    }
    // and handle the telemetry
    this->Telemetry::run();
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




uint8_t SerialBinaryCommander::writeByte(uint8_t value){
    return writeBytes(&value, 1);
};



uint8_t SerialBinaryCommander::writeFloat(float value){
    return writeBytes(&value, 4);
};



uint8_t SerialBinaryCommander::writeInt(uint32_t value){
    return writeBytes(&value, 4);
};




uint8_t SerialBinaryCommander::readByte(uint8_t* valueToSet){
    return readBytes(valueToSet, 1);
};



uint8_t SerialBinaryCommander::readFloat(float* valueToSet){
    return readBytes(valueToSet, 4);
};



uint8_t SerialBinaryCommander::readInt(uint32_t* valueToSet){
    return readBytes(valueToSet, 4);
};






void SerialBinaryCommander::sendHeader() {
    if (numRegisters > 0) {
        startFrame(numRegisters*2, TELEMETRY_FRAMETYPE_HEADER);
        for (uint8_t i = 0; i < numRegisters; i++) {
            writeByte(registers[i]);
            writeByte(registers_motor[i]);
        };
        endFrame();
    };
};



void SerialBinaryCommander::sendTelemetry(){
    if (numRegisters > 0) {
        startFrame(frameSize, TELEMETRY_FRAMETYPE_DATA);
        for (uint8_t i = 0; i < numRegisters; i++) {
            sendRegister(static_cast<SimpleFOCRegister>(registers[i]), motors[registers_motor[i]]);
        };
        endFrame();
    }
};
