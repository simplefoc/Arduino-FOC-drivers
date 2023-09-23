

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
    headerSent = false;
};



void Telemetry::run() {
    if (numRegisters<1)
        return;
    if (!headerSent) {
        sendHeader();
        headerSent = true;
    }
    if (downsampleCnt++ < downsample) return;
    downsampleCnt = 0;
    if (min_elapsed_time > 0) {
        long now = _micros();
        if (now - last_run_time < min_elapsed_time) return;
        last_run_time = now;
    }
    sendTelemetry();
}



void Telemetry::addMotor(FOCMotor* motor) {
    if (numMotors < TELEMETRY_MAX_MOTORS) {
        motors[numMotors] = motor;
        numMotors++;
    }
};

