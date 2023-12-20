
#pragma once

#include "common/base_classes/FOCMotor.h"
#include "../SimpleFOCRegisters.h"
#include "../RegisterIO.h"

#ifndef TELEMETRY_MAX_REGISTERS
#define TELEMETRY_MAX_REGISTERS 8
#endif

#ifndef TELEMETRY_MAX_MOTORS
#define TELEMETRY_MAX_MOTORS 4
#endif


#define DEF_TELEMETRY_DOWNSAMPLE 100


typedef enum : uint8_t {
    TELEMETRY_PACKETTYPE_DATA = 'T',
    TELEMETRY_PACKETTYPE_HEADER = 'H'
} TelemetryFrameType;




class Telemetry {
public:
    Telemetry(PacketIO& _comms);
    virtual ~Telemetry();
    virtual void init();
    void addMotor(FOCMotor* motor);
    void setTelemetryRegisters(uint8_t numRegisters, uint8_t* registers, uint8_t* motors = NULL);
    void run();

    uint16_t downsample = DEF_TELEMETRY_DOWNSAMPLE;
    uint32_t min_elapsed_time = 0;
protected:
    virtual void sendTelemetry();
    virtual void sendHeader();

    uint8_t id;

    FOCMotor* motors[TELEMETRY_MAX_MOTORS];
    uint8_t numMotors = 0;    

    uint8_t numRegisters;
    uint8_t registers[TELEMETRY_MAX_REGISTERS];
    uint8_t registers_motor[TELEMETRY_MAX_REGISTERS];
    uint8_t packetSize;
    bool headerSent;
    unsigned long last_run_time = 0;
    uint16_t downsampleCnt = 0;

    PacketIO& comms;
    
    static uint8_t id_seed; // TODO how to initialize this?
};
