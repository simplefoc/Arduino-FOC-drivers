
#include "./SimpleTelemetry.h"


SimpleTelemetry::SimpleTelemetry(){
};


SimpleTelemetry::~SimpleTelemetry(){
    // nothing to do here
};


void SimpleTelemetry::init(TextIO& _comms){
    this->Telemetry::init(_comms);
};


void SimpleTelemetry::sendTelemetry(){
    if (numRegisters > 0) {
        for (uint8_t i = 0; i < numRegisters; i++) {
            SimpleFOCRegisters::regs->registerToComms(*comms, registers[i], motors[registers_motor[i]]);
            if (i<numRegisters-1) *comms << '\t';
        };
        *comms << '\n';
    }
};


void SimpleTelemetry::sendHeader(){
    if (numRegisters > 0) {
        for (uint8_t i = 0; i < numRegisters; i++) {
            *comms << 'R'; 
            if (registers_motor[i] != 0) {
                *comms << registers_motor[i] << '_';
            }
            *comms << registers[i];
            if (i<numRegisters-1) *comms << '\t';
        };
        *comms << '\n';
    }
};
