

#include "./Telemetry.h"



Telemetry::Telemetry() {
    this->numRegisters = 0;
};



Telemetry::~Telemetry(){

};


uint8_t Telemetry::num_telemetry = 0;
uint8_t Telemetry::telemetry_ctrl = 0;
Telemetry* Telemetry::telemetries[TELEMETRY_MAX_TELEMETRIES];

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
    headerSent = false;
};



void Telemetry::init(PacketIO& _comms) {
    comms = &_comms;
    if (Telemetry::num_telemetry < TELEMETRY_MAX_TELEMETRIES) {
        Telemetry::telemetries[Telemetry::num_telemetry] = this;
    }
    this->id = Telemetry::num_telemetry++;
    headerSent = false;
    if (SimpleFOCRegisters::regs == NULL) {
        SimpleFOCRegisters::regs = new SimpleFOCRegisters();
    }
};



void Telemetry::run() {
    if (numRegisters<1)
        return;
    unsigned long now = 0;
    if (id==0) {
        now = _micros(); 
        iterations++;
        if (now - last_iter_time >= 1000000) { // TODO add overflow protection
            last_iter_time = now;
            last_iterations = iterations;
            iterations = 0;
        }
    }
    if (downsample==0)
        return;
    if (downsample==0 || downsampleCnt++ < downsample) return;
    downsampleCnt = 0;
    if (min_elapsed_time > 0) {
        if (id!=0) now = _micros();
        if (now - last_run_time < min_elapsed_time) return; // TODO add overflow protection
        last_run_time = now;
    }
    if (!headerSent) {
        sendHeader();
        headerSent = true;
    }
    sendTelemetry();
}



void Telemetry::addMotor(FOCMotor* motor) {
    if (numMotors < TELEMETRY_MAX_MOTORS) {
        motors[numMotors] = motor;
        numMotors++;
    }
};



void Telemetry::sendHeader() {
    if (numRegisters > 0) {
        *comms << START_PACKET(PacketType::TELEMETRY_HEADER, 2*numRegisters + 1) << id << Separator('=');
        for (uint8_t i = 0; i < numRegisters; i++) {
            *comms << registers_motor[i] << Separator(':') << registers[i];
        }
        *comms << END_PACKET;
    };
};



void Telemetry::sendTelemetry(){
    if (numRegisters > 0) {
        uint8_t size = 1;
        for (uint8_t i = 0; i < numRegisters; i++) {
            size += SimpleFOCRegisters::regs->sizeOfRegister(registers[i]);
        }
        *comms << START_PACKET(PacketType::TELEMETRY, size) << id << Separator('=');
        for (uint8_t i = 0; i < numRegisters; i++) {
            SimpleFOCRegisters::regs->registerToComms(*comms, registers[i], motors[registers_motor[i]]);
        };
        *comms << END_PACKET;
    }
};
