
#include "./TeleplotTelemetry.h"


TeleplotTelemetry::TeleplotTelemetry(){
};


TeleplotTelemetry::~TeleplotTelemetry(){
    // nothing to do here
};


void TeleplotTelemetry::init(TextIO& _comms){
    this->Telemetry::init(_comms);
};


void TeleplotTelemetry::sendTelemetry(){
    if (numRegisters > 0) {
        for (uint8_t i = 0; i < numRegisters; i++) {
            *comms << '>'; 
            if (registers_motor[i] != 0) {
                *comms << registers_motor[i] << '_';
            }
            *comms << registers[i] << ':';
            SimpleFOCRegisters::regs->registerToComms(*comms, registers[i], motors[registers_motor[i]]);
            *comms << '\n';
        };
    }
};


void TeleplotTelemetry::sendHeader(){
    // don't do anything
};
