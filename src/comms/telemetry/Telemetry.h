#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__


#include "../SimpleFOCRegisters.h"
#include "common/base_classes/FOCMotor.h"

#ifndef TELEMETRY_MAX_MOTORS
#define TELEMETRY_MAX_MOTORS 4
#endif

#ifndef TELEMETRY_MAX_REGISTERS
#define TELEMETRY_MAX_REGISTERS 8
#endif


enum : uint8_t {
    TELEMETRY_FRAMETYPE_DATA = 0x01,
    TELEMETRY_FRAMETYPE_HEADER = 0x02
} TelemetryFrameType;




class Telemetry {
public:
    Telemetry();
    virtual ~Telemetry();
    virtual void init();
    void addTelemetryMotor(FOCMotor& motor);
    void setTelemetryRegisters(uint8_t numRegisters, uint8_t* registers, uint8_t* motors = NULL);
    virtual void sendTelemetry() = 0;
protected:
    virtual void sendHeader() = 0;
    virtual void startFrame(uint8_t frameSize, uint8_t type = TELEMETRY_FRAMETYPE_DATA) = 0;
    virtual void endFrame() = 0;

    FOCMotor* motors[TELEMETRY_MAX_MOTORS];
    uint8_t numMotors = 0;

    uint8_t numRegisters;
    uint8_t registers[TELEMETRY_MAX_REGISTERS];
    uint8_t registers_motor[TELEMETRY_MAX_REGISTERS];
    uint8_t frameSize;
};



class BinaryTelemetry : public Telemetry {
public:
    BinaryTelemetry();
    virtual ~BinaryTelemetry();
    void sendTelemetry();
    void sendHeader();

    virtual bool sendRegister(SimpleFOCRegister reg, uint8_t motorNum = 0) = 0;
    virtual uint8_t writeByte(uint8_t value);

};



#endif