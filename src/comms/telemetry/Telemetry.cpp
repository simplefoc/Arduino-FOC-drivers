

#include "./Telemetry.h"



Telemetry::Telemetry() {
    this->numRegisters = 0;
};



Telemetry::~Telemetry(){

};





void Telemetry::setTelemetryRegisters(uint8_t numRegisters, uint8_t* registers, uint8_t* motors){
    if (numRegisters<=TELEMETRY_MAX_REGISTERS) {
        this->numRegisters = numRegisters;
        for (uint8_t i=0; i<numRegisters; i++) {
            this->registers[i] = registers[i];
            if (motors!=NULL)
                this->registers_motor[i] = motors[i];
            else
                this->registers_motor[i] = 0;
        }
    }
};



void Telemetry::init() {
    sendHeader();
};



BinaryTelemetry::BinaryTelemetry(RegisterSender* sender) : sender(sender) {
    // nix
};

BinaryTelemetry::~BinaryTelemetry(){

};


void BinaryTelemetry::sendHeader() {
    if (numRegisters > 0) {
        startFrame(numRegisters*2, TELEMETRY_FRAMETYPE_HEADER);
        for (uint8_t i = 0; i < numRegisters; i++) {
            sender->writeByte(registers[i]);
            sender->writeByte(registers_motor[i]);
        };
        endFrame();
    };
};



void BinaryTelemetry::sendTelemetry(){
    if (numRegisters > 0) {
        startFrame(frameSize, TELEMETRY_FRAMETYPE_DATA);
        for (uint8_t i = 0; i < numRegisters; i++) {
            sender->sendRegister(static_cast<SimpleFOCRegister>(registers[i]), registers_motor[i]);
        };
        endFrame();
    }
};


ASCIITelemetry::ASCIITelemetry(ASCIIRegisterSender* sender) : sender(sender) {
    // nix
};

ASCIITelemetry::~ASCIITelemetry(){

};

void ASCIITelemetry::sendHeader() {
    if (numRegisters > 0) {
        sender->writeChar('H');
        sender->writeChar(' ');
        for (uint8_t i = 0; i < numRegisters; i++) {
            sender->writeByte(registers_motor[i]);
            sender->writeChar(':');
            sender->writeByte(registers[i]);
            if (i < numRegisters-1)
                sender->writeChar(' ');
        };
        sender->writeChar('\n');
    };
};



void ASCIITelemetry::sendTelemetry(){
    if (numRegisters > 0) {
        for (uint8_t i = 0; i < numRegisters; i++) {
            sender->sendRegister(static_cast<SimpleFOCRegister>(registers[i]), registers_motor[i]);
            if (i < numRegisters-1)
                sender->writeChar(' ');
        };
        sender->writeChar('\n');
    }
};
