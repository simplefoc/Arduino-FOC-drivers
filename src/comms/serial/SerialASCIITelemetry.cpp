
#include "./SerialASCIITelemetry.h"

SerialASCIITelemetry::SerialASCIITelemetry(int floatPrecision) : Telemetry() {
    this->floatPrecision = floatPrecision;
};

SerialASCIITelemetry::~SerialASCIITelemetry(){

};

void SerialASCIITelemetry::init(Print* print){
    this->_print = print;
    this->Telemetry::init();
};



void SerialASCIITelemetry::sendHeader() {
    if (numRegisters > 0) {
        writeChar('H');
        writeChar(' ');
        for (uint8_t i = 0; i < numRegisters; i++) {
            writeByte(registers_motor[i]);
            writeChar(':');
            writeByte(registers[i]);
            if (i < numRegisters-1)
                writeChar(' ');
        };
        writeChar('\n');
    };
};



void SerialASCIITelemetry::sendTelemetry(){
    if (numRegisters > 0) {
        for (uint8_t i = 0; i < numRegisters; i++) {
            sendRegister(static_cast<SimpleFOCRegister>(registers[i]), motors[registers_motor[i]]);
            if (i < numRegisters-1)
                writeChar(' ');
        };
        writeChar('\n');
    }
};



uint8_t SerialASCIITelemetry::writeChar(char value){
    return _print->print(value);
};

uint8_t SerialASCIITelemetry::writeByte(uint8_t value){
    return _print->print(value);
};


uint8_t SerialASCIITelemetry::writeFloat(float value){
    return _print->print(value, floatPrecision);
};


uint8_t SerialASCIITelemetry::writeInt(uint32_t value){
    return _print->print(value);    
};