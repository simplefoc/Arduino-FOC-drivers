

#include "./Telemetry.h"



Telemetry::Telemetry(){

};



Telemetry::~Telemetry(){

};







void Telemetry::addTelemetryMotor(FOCMotor& motor){
    if (numMotors < TELEMETRY_MAX_MOTORS)
        motors[numMotors++] = &motor;
};



void Telemetry::setTelemetryRegisters(uint8_t numRegisters, uint8_t* registers, uint8_t* motors){
    this->numRegisters = numRegisters;
    // TODO
};



void Telemetry::init() {
    sendHeader();
};



void BinaryTelemetry::sendHeader() {
    if (numRegisters > 0) {
        startFrame(numRegisters*2, TELEMETRY_FRAMETYPE_HEADER);
        for (uint8_t i = 0; i < numRegisters; i++) {
            writeByte(registers[i]);
            writeByte(registers_motor[i]);
        };
        endFrame();
    };
};



void BinaryTelemetry::sendTelemetry(){
    startFrame(frameSize, TELEMETRY_FRAMETYPE_DATA);
    for (uint8_t i = 0; i < numRegisters; i++) {
        sendRegister(static_cast<SimpleFOCRegister>(registers[i]), registers_motor[i]);
    };
    endFrame();
};
